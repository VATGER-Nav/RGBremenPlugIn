#include "TagHandler.h"

TagHandler::TagHandler(Config* config, SidStarHandler* sidstarHandler, int transitionAltitude)
{
	m_Configuration = config;
	m_SidStarHandler = sidstarHandler;
	m_TransitionAltitude = transitionAltitude;
}

TagHandler::~TagHandler()
{
}

void TagHandler::SetTransitionAltitude(int transitionAltitude)
{
	m_TransitionAltitude = transitionAltitude;
}

TagItemContainer TagHandler::HandleTagItem(EuroScopePlugIn::CFlightPlan f, EuroScopePlugIn::CRadarTarget r, int tagItemCode)
{
	TagItemContainer tic = TagItemContainer();

	// Temporary char buffer
	char* tmp = new char[15];

	switch (tagItemCode)
	{
	case RG_BREMEN_TAG_ITEM_GROUNDSPEED:
		if (r.GetGS() / 10 < 100) {
			_itoa_s(r.GetGS() / 10, tmp, 15, 10);
			tic.sItemString = tmp;
		}
		else {
			tic.sItemString = "";
		}
		break;
	case RG_BREMEN_TAG_ITEM_ALTITUDE:
		if (r.GetPosition().GetFlightLevel() <= m_TransitionAltitude) {
			tic.sItemString = ("A" + padWithZeros(4, r.GetPosition().GetPressureAltitude())).substr(0, 3);
		}
		else {
			tic.sItemString = padWithZeros(5, r.GetPosition().GetFlightLevel()).substr(0, 3);
		}
		break;
	case RG_BREMEN_TAG_ITEM_VERTICAL_SPEED_INDICATOR:
		if (!SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			if (r.GetVerticalSpeed() / 200 < -2) {
				tic.sItemString = "|";
			}
			if (r.GetVerticalSpeed() / 200 > 2) {
				tic.sItemString = "^";
			}
		}
		break;
	case RG_BREMEN_TAG_ITEM_VERTICAL_SPEED:
		if (!SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			int vs = r.GetVerticalSpeed() / 200;
			vs = (vs < 0) ? vs * -1 : vs;
			if (vs <= 2) {
				tic.sItemString = "";
			}
			else if (vs < 10) {
				//_itoa_s(vs, tmp, 15, 10);
				tic.sItemString = padWithZeros(2, vs);
			}
			else {
				_itoa_s(vs, tmp, 15, 10);
				tic.sItemString = tmp;
			}
		}
		break;
	case RG_BREMEN_TAG_ITEM_INDICATED_AIRSPEED:
		if (!SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			if (r.GetPosition().GetReportedGS() / 10 <= 99) {
				tic.sItemString = ("N" + padWithZeros(2, r.GetPosition().GetReportedGS() / 10)).substr(0, 3);
			}
			else {
				tic.sItemString = "";
			}
		}
		else {
			tic.sItemString = "";
		}
		break;
	case RG_BREMEN_TAG_ITEM_DESTINATION:
		if (!SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			tic.sItemString = f.GetFlightPlanData().GetDestination();
		}
		else {
			tic.sItemString = "";
		}
		break;
	case RG_BREMEN_TAG_ITEM_SIDSTAR:
		if (!SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			tic.customColor = true;
			if (f.GetDistanceToDestination() > 100.0 && f.GetDistanceFromOrigin() < f.GetDistanceToDestination()) {
				if (m_SidStarHandler->AnalyzeSid(f)) {
					tic.color = RGB(m_Configuration->GetListColorSidStarYes()[0], m_Configuration->GetListColorSidStarYes()[1], m_Configuration->GetListColorSidStarYes()[2]);
				}
				tic.sItemString = f.GetFlightPlanData().GetSidName();
			}
			else {
				if (m_SidStarHandler->AnalyzeStar(f)) {
					tic.color = RGB(m_Configuration->GetListColorSidStarYes()[0], m_Configuration->GetListColorSidStarYes()[1], m_Configuration->GetListColorSidStarYes()[2]);
				}
				tic.sItemString = f.GetFlightPlanData().GetStarName();
			}
		}
		break;
	case RG_BREMEN_TAG_ITEM_CLEARED_FLIGHT_LEVEL:
		if (!SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk())) && f.GetFlightPlanData().GetPlanType()[0] == 'I') {
			int cfl = f.GetControllerAssignedData().GetClearedAltitude();

			if (cfl == 0) cfl = f.GetFinalAltitude();
			if (cfl == 1) {
				tic.sItemString = "APP";
			}
			else if (cfl == 2) {
				tic.sItemString = "VIS";
			}
			else if (cfl > 9999) {
				_itoa_s(cfl / 100, tmp, 15, 10);
				tic.sItemString = tmp;
			}
			else if (cfl > 99) {
				tic.sItemString = padWithZeros(3, cfl / 100).substr(0, 3);
			}
			else {
				tic.sItemString = "000";
			}
		}
		break;
	case RG_BREMEN_TAG_ITEM_ASSIGNED_HEADING:
		if (!SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk())) && f.GetFlightPlanData().GetPlanType()[0] == 'I') {
			if (f.GetControllerAssignedData().GetDirectToPointName() != "") {
				tic.sItemString = f.GetControllerAssignedData().GetDirectToPointName();
			}
			else if (f.GetControllerAssignedData().GetAssignedHeading() != 0) {
				tic.sItemString = "H" + padWithZeros(3, f.GetControllerAssignedData().GetAssignedHeading());
			}
		}
		break;
	/*case RG_BREMEN_TAG_ITEM_COPX:
		tic.sItemString = "WIP";
		break;
	case RG_BREMEN_TAG_ITEM_TRANSFER_FLIGHT_LEVEL:
		tic.sItemString = "WIP";
		break;
	case RG_BREMEN_TAG_ITEM_NEXT_SECTOR:
		tic.sItemString = "WIP";
		break;*/
	case RG_BREMEN_TAG_ITEM_VFR_INDICATOR:
		if (SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			tic.customColor = true;
			tic.color = RGB(m_Configuration->GetTagColorVFR()[0], m_Configuration->GetTagColorVFR()[1], m_Configuration->GetTagColorVFR()[2]);
			switch (atoi(r.GetPosition().GetSquawk()))
			{
			case 0:
				tic.sItemString = "0000"; break;
			case 20:
				tic.sItemString = "RESCU"; break;
			case 23:
				tic.sItemString = "BPO"; break;
			case 24:
				tic.sItemString = "TFFN"; break;
			case 25:
				tic.sItemString = "PJE"; break;
			case 27:
				tic.sItemString = "ACRO"; break;
			case 30:
				tic.sItemString = "CAL"; break;
			case 31:
				tic.sItemString = "OPSKY"; break;
			case 33:
				tic.sItemString = "VM"; break;
			case 34:
				tic.sItemString = "SAR"; break;
			case 35:
				tic.sItemString = "AIRCL"; break;
			case 36:
				tic.sItemString = "POL"; break;
			case 37:
				tic.sItemString = "BIV"; break;
			case 76:
				tic.sItemString = "VFRCD"; break;
			case 1200:
				tic.sItemString = "1200"; break;
			case 2000:
				tic.sItemString = "2000"; break;
			case 2200:
				tic.sItemString = "2200"; break;
			case 7000:
				tic.sItemString = "V"; break;
			case 7740:
				tic.sItemString = "FIS"; break;
			case 7741:
				tic.sItemString = "FIS"; break;
			case 7742:
				tic.sItemString = "FIS"; break;
			case 7743:
				tic.sItemString = "FIS"; break;
			case 7744:
				tic.sItemString = "FIS"; break;
			case 7745:
				tic.sItemString = "FIS"; break;
			default:
				tic.sItemString = r.GetCallsign();  break;
			}
		}
		else {
			switch (atoi(r.GetPosition().GetSquawk()))
			{
			case 1200:
				tic.sItemString = "1200"; break;
			case 2000:
				tic.sItemString = "2000"; break;
			case 2200:
				tic.sItemString = "2200"; break;
			default:
				tic.sItemString = r.GetCallsign(); break;
			}

		}
		break;
	case RG_BREMEN_TAG_ITEM_VFR_CALLSIGN:
		tic.sItemString = r.GetCallsign();
		if(SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			tic.customColor = true;
			tic.color = RGB(m_Configuration->GetTagColorVFR()[0], m_Configuration->GetTagColorVFR()[1], m_Configuration->GetTagColorVFR()[2]);
		}
		break;
	case RG_BREMEN_TAG_ITEM_VFR_ALTITUDE:
		if (SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			tic.customColor = true;
			tic.color = RGB(m_Configuration->GetTagColorVFR()[0], m_Configuration->GetTagColorVFR()[1], m_Configuration->GetTagColorVFR()[2]);
		}
		if (r.GetPosition().GetFlightLevel() <= m_TransitionAltitude) {
			tic.sItemString = ("A" + padWithZeros(4, r.GetPosition().GetPressureAltitude())).substr(0, 3);
		}
		else {
			tic.sItemString = padWithZeros(5, r.GetPosition().GetFlightLevel()).substr(0, 3);
		}
		break;
	case RG_BREMEN_TAG_ITEM_VFR_INDICATED_AIRSPEED:
		if (!SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			if (r.GetPosition().GetReportedGS() / 10 <= 99) {
				tic.sItemString = ("N" + padWithZeros(2, r.GetPosition().GetReportedGS() / 10)).substr(0, 3);
			}
			else {
				tic.sItemString = "";
			}
		}
		else {
			tic.customColor = true;
			tic.color = RGB(m_Configuration->GetTagColorVFR()[0], m_Configuration->GetTagColorVFR()[1], m_Configuration->GetTagColorVFR()[2]);
			tic.sItemString = "";
		}
		break;
	case RG_BREMEN_TAG_ITEM_TWR_INDICATOR:
		if (SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			tic.customColor = true;
			tic.color = RGB(m_Configuration->GetTagColorTWR()[0], m_Configuration->GetTagColorTWR()[1], m_Configuration->GetTagColorTWR()[2]);
			switch (atoi(r.GetPosition().GetSquawk()))
			{
			case 0:
				tic.sItemString = "0000"; break;
			case 20:
				tic.sItemString = "RESCU"; break;
			case 23:
				tic.sItemString = "BPO"; break;
			case 24:
				tic.sItemString = "TFFN"; break;
			case 25:
				tic.sItemString = "PJE"; break;
			case 27:
				tic.sItemString = "ACRO"; break;
			case 30:
				tic.sItemString = "CAL"; break;
			case 31:
				tic.sItemString = "OPSKY"; break;
			case 33:
				tic.sItemString = "VM"; break;
			case 34:
				tic.sItemString = "SAR"; break;
			case 35:
				tic.sItemString = "AIRCL"; break;
			case 36:
				tic.sItemString = "POL"; break;
			case 37:
				tic.sItemString = "BIV"; break;
			case 76:
				tic.sItemString = "VFRCD"; break;
			case 1200:
				tic.sItemString = "1200"; break;
			case 2000:
				tic.sItemString = "2000"; break;
			case 2200:
				tic.sItemString = "2200"; break;
			case 7000:
				tic.sItemString = "V"; break;
			case 7740:
				tic.sItemString = "FIS"; break;
			case 7741:
				tic.sItemString = "FIS"; break;
			case 7742:
				tic.sItemString = "FIS"; break;
			case 7743:
				tic.sItemString = "FIS"; break;
			case 7744:
				tic.sItemString = "FIS"; break;
			case 7745:
				tic.sItemString = "FIS"; break;
			default:
				tic.sItemString = r.GetCallsign();  break;
			}
		}
		else {
			switch (atoi(r.GetPosition().GetSquawk()))
			{
			case 1200:
				tic.sItemString = "1200"; break;
			case 2000:
				tic.sItemString = "2000"; break;
			case 2200:
				tic.sItemString = "2200"; break;
			default:
				tic.sItemString = r.GetCallsign(); break;
			}

		}
		break;
	case RG_BREMEN_TAG_ITEM_TWR_CALLSIGN:
		if (SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			tic.color = RGB(m_Configuration->GetTagColorTWR()[0], m_Configuration->GetTagColorTWR()[1], m_Configuration->GetTagColorTWR()[2]);
			tic.customColor = true;
		}
		tic.sItemString = r.GetCallsign();
		break;
	case RG_BREMEN_TAG_ITEM_TWR_ALTITUDE:
		if (SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			tic.color = RGB(m_Configuration->GetTagColorTWR()[0], m_Configuration->GetTagColorTWR()[1], m_Configuration->GetTagColorTWR()[2]);
			tic.customColor = true;
		}
		if (r.GetPosition().GetFlightLevel() <= m_TransitionAltitude) {
			tic.sItemString = ("A" + padWithZeros(4, r.GetPosition().GetPressureAltitude())).substr(0, 3);
		}
		else {
			tic.sItemString = padWithZeros(5, r.GetPosition().GetFlightLevel()).substr(0, 3);
		}
		break;
	case RG_BREMEN_TAG_ITEM_TWR_INDICATED_AIRSPEED:
		if (!SquawkHandler::IsSquawkVFR(atoi(r.GetPosition().GetSquawk()))) {
			if (r.GetPosition().GetReportedGS() / 10 <= 99) {
				tic.sItemString = ("N" + padWithZeros(2, r.GetPosition().GetReportedGS() / 10)).substr(0, 3);
			}
			else {
				tic.sItemString = "";
			}
		}
		else {
			tic.color = RGB(m_Configuration->GetTagColorTWR()[0], m_Configuration->GetTagColorTWR()[1], m_Configuration->GetTagColorTWR()[2]);
			tic.customColor = true;
			tic.sItemString = "";
		}
		break;
	default:
		tic.handled = false;
		break;
	}

	return tic;
}
