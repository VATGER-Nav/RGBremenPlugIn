#include "DeliveryHelper.h"

DeliveryHelper::DeliveryHelper(Config* config) : m_Config(config)
{
	try {
		nlohmann::json j = m_Config->GetFlightPlanConfiguration().at("settings");

		assignNap = j.at("assignNAP");
		autoProcess = j.at("autoProcess");
		warnRFLBelowCFL = j.at("warnRFLBelowCFL");
		logMinMaxRFL = j.at("logMinMaxRFL");
		checkMinMaxRFL = j.at("checkMinMaxRFL");
	}
	catch (std::exception& ex) {
		m_MessageMap.push_back("Failed loading flight plan configuration from flightplan.settings");
		m_MessageMap.push_back(ex.what());
		return;
	}

	this->ProcessAirportConfig();
	this->ProcessRoutingConfig();
}

DeliveryHelper::~DeliveryHelper()
{
}

std::string DeliveryHelper::GetNextMessage()
{
	if (m_MessageMap.size() > 0) {
		std::string msg = m_MessageMap.at(0);
		m_MessageMap.erase(m_MessageMap.begin());
		return msg;
	}
	return "No message";
}

bool DeliveryHelper::HasMessage()
{
	return m_MessageMap.size() > 0;
}

void DeliveryHelper::ProcessAirportConfig()
{
	try
	{
		nlohmann::json ja = m_Config->GetFlightPlanConfiguration().at("airports");

		for (auto& [icao, airportConfig] : ja.items()) {
			m_MessageMap.push_back("Parsing airport " + icao);
			Airport airport{
				icao,
				airportConfig.value<int>("elevation", 0),
				false
			};

			// Handle sids
			try
			{
				nlohmann::json jas = airportConfig.at("sids");

				for (auto& [waypoint, sidConfig] : jas.items()) {
					Sid s{
						waypoint,
						sidConfig.value<int>("cfl", 0)
					};

					try {
						nlohmann::json jsrwys = sidConfig.at("rwys");

						std::ostringstream rrs;
						rrs << icao << "\\/(";

						for (auto& [ident, rwyConfig] : jsrwys.items()) {
							try
							{
								for (const auto& sid : rwyConfig.at("dep")) {
									SidInfo si{
										ident,
										sid,
										rwyConfig.value<std::string>("nap", ""),
										rwyConfig.value<int>("prio", 0)
									};
									s.rwys.emplace(si.rwy, si);
									airport.rwys.emplace(si.rwy, false);
									rrs << si.rwy;
								}
								rrs << "|";
							}
							catch (const std::exception& e)
							{
								m_MessageMap.push_back(e.what());
							}
						}

						rrs << ")";

						airport.rwy_regex = std::regex(rrs.str(), std::regex_constants::ECMAScript);
						airport.sids.emplace(waypoint, s);
						
					}
					catch (const std::exception& e) {
						m_MessageMap.push_back(e.what());
					}
				}
			}
			catch (const std::exception& e)
			{
				m_MessageMap.push_back(e.what());
			}

			this->m_Airports.emplace(icao, airport);
		}
	}
	catch (const std::exception& e)
	{
		m_MessageMap.push_back(e.what());
	}
}

void DeliveryHelper::ProcessRoutingConfig()
{
}

void DeliveryHelper::UpdateActiveAirports(EuroScopePlugIn::CSectorElement& sfe)
{
	if (!sfe.IsValid() || sfe.GetElementType() != EuroScopePlugIn::SECTOR_ELEMENT_AIRPORT) return;

	std::string ap = sfe.GetAirportName();
	trim(ap);
	toUppercase(ap);

	auto ait = this->m_Airports.find(ap);
	if (ait == this->m_Airports.end()) {
		return;
	}

	std::string rwy = sfe.GetRunwayName(0);
	trim(rwy);
	toUppercase(rwy);

	auto rit = ait->second.rwys.find(rwy);
	if (rit != ait->second.rwys.end()) {
		rit->second = sfe.IsElementActive(true, 0);
	}

	rwy = sfe.GetRunwayName(1);
	trim(rwy);
	toUppercase(rwy);

	rit = ait->second.rwys.find(rwy);
	if (rit != ait->second.rwys.end()) {
		rit->second = sfe.IsElementActive(true, 1);
	}

	if (!ait->second.active) {
		ait->second.active = sfe.IsElementActive(true, 0) || sfe.IsElementActive(true, 1);
	}
}

void DeliveryHelper::FlightPlanClosed(EuroScopePlugIn::CFlightPlan& fp)
{
	this->m_Processed.erase(std::remove(this->m_Processed.begin(), this->m_Processed.end(), fp.GetCallsign()), this->m_Processed.end());
}

Validation DeliveryHelper::ProcessFlightPlan(EuroScopePlugIn::CFlightPlan& fp, bool nap, bool validateOnly)
{
	Validation v{
		true,
		"",
		RGB(200, 200, 200)
	};

	std::string callsign = fp.GetCallsign();
	EuroScopePlugIn::CFlightPlanData fpd = fp.GetFlightPlanData();

	std::string dep = fpd.GetOrigin();
	toUppercase(dep);

	std::string arr = fpd.GetDestination();
	toUppercase(arr);

	/*const COLORREF TAG_COLOR_NONE = 0;
	const COLORREF TAG_COLOR_RED = RGB(200, 0, 0);
	const COLORREF TAG_COLOR_ORANGE = RGB(255, 165, 0);
	const COLORREF TAG_COLOR_GREEN = RGB(0, 200, 0);*/

	auto ait = this->m_Airports.find(dep);
	if (ait == this->m_Airports.end()) {
		// Failed to find departure aerodrome
		v.valid = false;
		v.tag = "ADEP";
		v.color = RGB(200, 0, 0);

		return v;
	}

	Airport airport = ait->second;
	EuroScopePlugIn::CFlightPlanControllerAssignedData cad = fp.GetControllerAssignedData();

	if (strcmp(fpd.GetPlanType(), "V") == 0 || strcmp(fpd.GetPlanType(), "Z") == 0) {
		if (!validateOnly) {
			if (!cad.SetClearedAltitude(roundToNearest((double)airport.elevation + VFR_TRAFFIC_PATTERN_ALTITUDE, 500))) {
				return v;
			}

			// Add to list of processed flightplans if not added by auto-processing already
			this->IsFlightPlanProcessed(fp);
		}

		v.tag = "VFR";

		return v;
	}

	std::vector<std::string> route = split(fpd.GetRoute());
	Sid sid;

	auto rit = route.begin();
	while (rit != route.end()) {
		if (std::regex_search(*rit, airport.rwy_regex)) {
			++rit;
			v.tag = "RWY";
			continue;
		}

		std::map<std::string, ::Sid>::iterator sit;
		std::smatch m;
		if (std::regex_search(*rit, m, REGEX_SPEED_LEVEL_GROUP)) {
			// Try to match waypoint of speed/level group in case SID fix already has one assigned
			sit = airport.sids.find(m[1]);
		}
		else {
			// If no other matchers above yield a result, try to match full route part
			sit = airport.sids.find(*rit);
		}

		if (sit != airport.sids.end()) {
			sid = sit->second;
			break;
		}

		rit = route.erase(rit);
	}

	if (sid.wp == "" || route.size() == 0) {
		if (!validateOnly) {
			m_MessageMap.push_back("Invalid flightplan, no valid SID waypoint found in route " + callsign);
		}

		v.valid = false;
		v.tag = "SID";
		v.color = RGB(200, 0, 0);

		return v;
	}

	if (validateOnly) {
		if (this->warnRFLBelowCFL && fp.GetFinalAltitude() < sid.cfl) {
			v.tag = "RFL";
			v.color = RGB(255, 165, 0);

			return v;
		}

		int cfl = cad.GetClearedAltitude();
		// If CFL == RFL, EuroScope returns a CFL of 0 and the RFL value should be consulted. Additionally, CFL 1 and 2 indicate ILS and visual approach clearances respectively.
		if (cfl < 3) {
			// If the RFL is not adapted or confirmed by the controller, cad.GetFinalAltitude() will also return 0. As a last source of CFL info, we need to consider the filed RFL.
			cfl = cad.GetFinalAltitude();
			if (cfl < 3) {
				cfl = fp.GetFinalAltitude();
			}
		}

		// Display a warning if the CFL does not match the initial CFL assigned to the SID. No warning is shown if the RFL is below the CFL for the SID as pilots might request a lower initial climb.
		if (cfl != sid.cfl && (cfl != fp.GetFinalAltitude() || fp.GetFinalAltitude() >= sid.cfl)) {
			v.valid = false;
			v.tag = "CFL";

			return v;
		}
	}
	else {
		if (!fpd.SetRoute(join(route).c_str())) {
			m_MessageMap.push_back("Failed to process flightplan, cannot set cleaned route " + callsign);
			return v;
		}

		if (!fpd.AmendFlightPlan()) {
			m_MessageMap.push_back("Failed to process flightplan, cannot amend flightplan after setting cleaned route " + callsign);
			return v;
		}

		std::map<std::string, SidInfo>::iterator sit{};
		std::string rwy = fpd.GetDepartureRwy();
		if (rwy == "") {
			//this->LogDebugMessage("No runway assigned, attempting to pick first active runway for SID", cs);

			// SIDs can have a priority assigned per runway, allowing for "hierarchy" depending on runway config (as currently possible in ES sectorfiles).
			// If no priority is assigned, the default of 0 will be used and the first active runway will be picked.
			int prio = -1;
			for (auto [r, active] : airport.rwys) {
				if (active) {
					auto s = sid.rwys.find(r);
					if (s != sid.rwys.end() && s->second.prio > prio) {
						sit = s;
						rwy = r;
						prio = sit->second.prio;
					}
				}
			}

			if (rwy == "") {
				//this->LogMessage("Failed to process flightplan, no runway assigned", cs);

				v.valid = false;
				v.tag = "RWY";
				v.color = RGB(200, 0, 0);

				return v;
			}

			// TODO display warning once "valid" tag override below is fixed
			/*v.tag = "SID";
			v.color = TAG_COLOR_GREEN;*/
		}
		else {
			sit = sid.rwys.find(rwy);
		}

		if (sit == sid.rwys.end()) {
			//this->LogMessage("Invalid flightplan, no matching SID found for runway", cs);

			v.valid = false;
			v.tag = "SID";
			v.color = RGB(200, 0, 0);

			return v;
		}

		SidInfo sidinfo = sit->second;

		std::ostringstream sssid;
		if (nap && sidinfo.nap != "") {
			sssid << sidinfo.nap;
		}
		else {
			sssid << sidinfo.dep;
		}
		sssid << "/" << rwy;

		route.insert(route.begin(), sssid.str());

		if (!fpd.SetRoute(join(route).c_str())) {
			//this->LogMessage("Failed to process flightplan, cannot set route including SID", cs);
			return v;
		}

		if (!fpd.AmendFlightPlan()) {
			//this->LogMessage("Failed to process flightplan, cannot amend flightplan after setting route including SID", cs);
			return v;
		}

		int cfl = sid.cfl;
		if (fp.GetFinalAltitude() < sid.cfl) {
			//this->LogDebugMessage("Flightplan has RFL below initial CFL for SID, setting RFL", cs);

			cfl = fp.GetFinalAltitude();
		}

		if (!cad.SetClearedAltitude(cfl)) {
			//this->LogMessage("Failed to process flightplan, cannot set cleared flightlevel", cs);
			return v;
		}

		//this->LogDebugMessage("Successfully processed flightplan", cs);

		// Add to list of processed flightplans if not added by auto-processing already
		this->IsFlightPlanProcessed(fp);
	}


	if (airport.validroutes.size() != 0) {

		FlightPlan fpl = FlightPlan(fp.GetCallsign(), fp.GetExtractedRoute(), fpd.GetRoute()); // create fp for route validation

		bool routecheck = false;
		int count = 0;
		for (auto vait = airport.validroutes.begin(); vait != airport.validroutes.end(); ++vait) {

			routecheck = false;
			auto selsidit = vait->waypts.begin();

			if (*selsidit == sid.wp) {
				if (vait->waypts.size() > 1) {
					try {


						count = 0; //counter to disregard previous found waypoints in fpl
						for (auto wyprouit = vait->waypts.begin(); wyprouit != vait->waypts.end(); ++wyprouit) {
							for (auto wypfpl = fpl.route.begin() + count; wypfpl != fpl.route.end(); ++wypfpl) {

								if (wypfpl->airway && wypfpl->name.rfind(*wyprouit) == 0) { // check if waypoint name is part of the airway (e.g. SID)

									routecheck = true;
									++count;
									break;
								}
								if (*wyprouit == wypfpl->name) {
									routecheck = true;
									++count;
									break;
								}
								else {
									routecheck = false;
								}
								++count;
							}
							if (!routecheck) {
								break;
							}

						}
					}
					catch (std::exception e) {
						//this->LogDebugMessage("Error, No Routing", cs);
					}
				}
				else {
					routecheck = true;
				}
				if (routecheck && vait->ades == arr) { //check specified destinations like LOWI, LOWS, etc.

					if (this->checkMinMaxRFL && ((cad.GetFinalAltitude() == 0 && fpd.GetFinalAltitude() > vait->maxlvl * 100) || cad.GetFinalAltitude() > vait->maxlvl * 100)) {

						v.valid = false;
						v.tag = "MAX";
						v.color = RGB(255, 165, 0);

						if (!validateOnly) {
							std::ostringstream msg;
							msg << "Flights from " << dep << " to " << arr << " via " << sid.wp << " have a maximum FL of " << vait->maxlvl;

							if (this->logMinMaxRFL) {
								//this->LogMessage(msg.str(), cs);
							}
							else {
								//this->LogDebugMessage(msg.str(), cs);
							}
						}

						return v;
					}
					if (this->checkMinMaxRFL && ((cad.GetFinalAltitude() == 0 && fpd.GetFinalAltitude() < vait->minlvl * 100) || (cad.GetFinalAltitude() != 0 && cad.GetFinalAltitude() < vait->minlvl * 100))) {

						v.valid = false;
						v.tag = "MIN";
						v.color = RGB(255, 165, 0);

						if (!validateOnly) {
							std::ostringstream msg;
							msg << "Flights from " << dep << " to " << arr << " via " << sid.wp << " have a minimum FL of " << vait->minlvl;

							if (this->logMinMaxRFL) {
								//this->LogMessage(msg.str(), cs);
							}
							else {
								//this->LogDebugMessage(msg.str(), cs);
							}
						}

						return v;
					}

					//case all correct
					v.valid = true;
					v.tag = "";
					v.color = RGB(200, 200, 200);;

					return v;

				}
				else if (routecheck && vait->ades != arr && vait->ades == "") { // check for non specified destinations
					if (this->checkMinMaxRFL && ((cad.GetFinalAltitude() == 0 && fpd.GetFinalAltitude() > vait->maxlvl * 100) || cad.GetFinalAltitude() > vait->maxlvl * 100)) {

						v.valid = false;
						v.tag = "MAX";
						v.color = RGB(255, 165, 0);

						//if (!validateOnly) {
						//	std::ostringstream msg;
						//	msg << "Flights from " << dep << " via " << sid.wp << " have a maximum FL of " << vait->maxlvl;

						//	if (this->logMinMaxRFL) {
						//		//this->LogMessage(msg.str(), cs);
						//	}
						//	else {
						//		//this->LogDebugMessage(msg.str(), cs);
						//	}
						//}

						break;
					}
					if (this->checkMinMaxRFL && ((cad.GetFinalAltitude() == 0 && fpd.GetFinalAltitude() < vait->minlvl * 100) || (cad.GetFinalAltitude() != 0 && cad.GetFinalAltitude() < vait->minlvl * 100))) {

						v.valid = false;
						v.tag = "MIN";
						v.color = RGB(255, 165, 0);

						//if (!validateOnly) {
						//	std::ostringstream msg;
						//	msg << "Flights from " << dep << " via " << sid.wp << " have a minimum FL of " << vait->minlvl;

						//	if (this->logMinMaxRFL) {
						//		//this->LogMessage(msg.str(), cs);
						//	}
						//	else {
						//		//this->LogDebugMessage(msg.str(), cs);
						//	}
						//}

						break;
					}

					//case all correct
					v.valid = true;
					v.tag = "";
					v.color = RGB(200, 200, 200);

					return v;

				}
				else if (this->CheckFlightPlanProcessed(fp)) {
					v.valid = false;
					v.tag = "INV";
					v.color = RGB(255, 165, 0);

					continue;
				}
				else {
					v.valid = false;
					v.tag = "";
					v.color = RGB(200, 200, 200);
					continue;
				}
			}
		}
	}

	return v;
}

bool DeliveryHelper::CheckFlightPlanProcessed(EuroScopePlugIn::CFlightPlan& fp)
{
	std::string callsign = fp.GetCallsign();

	if (std::find(this->m_Processed.begin(), this->m_Processed.end(), callsign) != this->m_Processed.end()) {
		return true;
	}

	return false;
}

bool DeliveryHelper::IsFlightPlanProcessed(EuroScopePlugIn::CFlightPlan& fp)
{
	if (this->CheckFlightPlanProcessed(fp)) return true;

	this->m_Processed.push_back(fp.GetCallsign());

	return false;
}
