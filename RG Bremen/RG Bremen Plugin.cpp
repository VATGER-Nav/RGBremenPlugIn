#include "pch.h"
#include "RG Bremen Plugin.h"

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

bool CRGBremenPlugin::isVFR(int squawk)
{
	switch (squawk)
	{
		case 0:
		case 20:
		case 23:
		case 24:
		case 25:
		case 27:
		case 30:
		case 31:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 76:
		/*case 1200:
		case 2000:
		case 2200:*/
		case 7000:
		case 7740:
		case 7741:
		case 7742:
		case 7743:
		case 7744:
		case 7745:
			return true;
		default:
			return false;
	}
	return false;
}

void CRGBremenPlugin::loadConfiguration()
{
	string directory = getCurrentDirectory();

	ifstream cfgFile(directory + "\\RG Bremen.cfg");

	if (!cfgFile.is_open()) {
		cfgFile.close();
		return;
	}

	string line;
	while (getline(cfgFile, line)) {
		stringstream lineStream(line);
		string key;
		string value;
		getline(lineStream, key, '=');
		getline(lineStream, value);

		stringstream colorStream(value);
		string r, g, b;
		getline(colorStream, r, ',');
		getline(colorStream, g, ',');
		getline(colorStream, b);
		if (key == "URL_SIDSTAR_FILE")
		{
			url_sidstar_file_location = value;
		}
		if (key == "COLOR_VFR") {
			colorVfr = RGB(stoi(r), stoi(g), stoi(b));
		}
		if (key == "COLOR_TWR") {
			colorTwr = RGB(stoi(r), stoi(g), stoi(b));
		}
		if (key == "COLOR_HANDOFF") {
			colorHandoff = RGB(stoi(r), stoi(g), stoi(b));
		}
		if (key == "COLOR_SID_CLB_YES") {
			colorClimbViaSidYes = RGB(stoi(r), stoi(g), stoi(b));
		}
		if (key == "COLOR_SID_CLB_NO") {
			colorClimbViaSidNo = RGB(stoi(r), stoi(g), stoi(b));
		}
		/*if (key == "COLOR_CONCERNED") {
			colorConcerned = RGB(stoi(r), stoi(g), stoi(b));
		}
		if (key == "COLOR_UNCONCERNED") {
			colorUnconcerned = RGB(stoi(r), stoi(g), stoi(b));
		}
		if (key == "COLOR_ASSUMED") {
			colorAssumed = RGB(stoi(r), stoi(g), stoi(b));
		}*/
	}

	cfgFile.close();
}

void CRGBremenPlugin::readSidStarRestrictionsList()
{
	HRESULT hr;
	string localFilePath = getCurrentDirectory() + "\\/clbdesc_sidstar.txt";
	hr = URLDownloadToFile(NULL, url_sidstar_file_location.c_str(), localFilePath.c_str(), 0, NULL);
	if (hr == S_OK) {
		// Download completed wihout failure
		string directory = getCurrentDirectory();

		ifstream cfgFile(directory + "\\clbdesc_sidstar.txt");

		if (!cfgFile.is_open()) {
			cfgFile.close();
			return;
		}

		string line;
		while (getline(cfgFile, line)) {
			if (line.substr(0, 1) == ";") continue; // Skip comments
			stringstream lineStream(line);
			// Read the file
			string icao;
			string type;
			string point;
			string id;
			getline(lineStream, icao, ' ');
			getline(lineStream, type, ' ');
			getline(lineStream, point, ' ');
			getline(lineStream, id);
			
			if (type == "SID") {
				SID_CLB_DATA s;
				s.airport = icao;
				s.sid = point;
				s.id = id;
				climbViaSids.push_back(s);
			}
			if (type == "STAR") {
				STAR_DESC_DATA s;
				s.airport = icao;
				s.star = point;
				s.id = id;
				descendViaStar.push_back(s);
			}
			//DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, line.c_str(), true, true, false, false, false);
		}
		cfgFile.close();
	}
	else {
		DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, "Failed to load SID/STAR configuration.", true, true, true, false, false);
	}
	
}

std::string CRGBremenPlugin::getCurrentDirectory()
{
	char path[MAX_PATH];

	HMODULE hm = NULL;

	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR) "RG Bremen", &hm);

	GetModuleFileName(hm, path, sizeof(path));

	string dir(path);
	return dir.substr(0, dir.find_last_of("\\/"));
}

bool CRGBremenPlugin::analzyeSid(CFlightPlan fp)
{
	if (!fp.IsValid()) return false;

	if (climbViaSids.empty()) return false;

	for each (auto sid in climbViaSids)
	{
		if (fp.GetFlightPlanData().GetSidName() == sid.sid + sid.id) {
			return true;
		}
	}

	return false;
}

bool CRGBremenPlugin::analzyeStarTrans(CFlightPlan fp)
{
	if (!fp.IsValid()) return false;

	if (descendViaStar.empty()) return false;

	for each (auto star in descendViaStar) {
		if (fp.GetFlightPlanData().GetStarName() == star.star + star.id) {
			return true;
		}
	}

	return false;
}

vector<string> CRGBremenPlugin::Tokenize(const string str, const std::regex regex)
{
	vector<string> result;
	sregex_token_iterator it(str.begin(), str.end(), regex, -1);
	sregex_token_iterator reg_end;
	for (; it != reg_end; ++it) {
		if (!it->str().empty())
			result.emplace_back(it->str());
	}
	return result;
}

CRGBremenPlugin::CRGBremenPlugin() : CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE,
	PLUGIN_NAME,
	PLUGIN_VERSION,
	PLUGIN_DEVELOPER,
	PLUGIN_COPYRIGHT)
{
	// Define colors for default values before trying to read config
	colorVfr = RG_BREMEN_TAG_COLOR_VFR;
	colorTwr = RG_BREMEN_TAG_COLOR_TWR;
	colorHandoff = RG_BREMEN_TAG_COLOR_HANDOFF;
	colorClimbViaSidYes = RGB(0, 255, 0);
	colorClimbViaSidNo = RGB(255, 0, 0);
	/*colorConcerned = RG_BREMEN_TAG_COLOR_CONCERNED;
	colorUnconcerned = RG_BREMEN_TAG_COLOR_UNCONCERNED;
	colorAssumed = RG_BREMEN_TAG_COLOR_ASSUMED;*/

	loadConfiguration();

	climbViaSids = list<SID_CLB_DATA>(); // Init climb via sids empty
	descendViaStar = list<STAR_DESC_DATA>(); // Init desc via star empty
	readSidStarRestrictionsList();

	RegisterTagItemType("TAG / VFR Indicator", RG_BREMEN_TAG_ITEM_VFR_INDICATOR);
	RegisterTagItemType("TAG / VFR Callsign", RG_BREMEN_TAG_ITEM_VFR_CALLSIGN);
	RegisterTagItemType("TAG / TWR Callsign", RG_BREMEN_TAG_ITEM_TWR_CALLSIGN);
	RegisterTagItemType("TAG / Groundspeed", RG_BREMEN_TAG_ITEM_GROUNDSPEED);
	RegisterTagItemType("TAG / Altitude", RG_BREMEN_TAG_ITEM_ALTITUDE);
	RegisterTagItemType("TAG / VS Indicator", RG_BREMEN_TAG_ITEM_VS_INDICATOR);
	RegisterTagItemType("TAG / VS Speed", RG_BREMEN_TAG_ITEM_VS_SPEED);
	RegisterTagItemType("TAG / IAS", RG_BREMEN_TAG_ITEM_IAS);
	RegisterTagItemType("TAG / Destination", RG_BREMEN_TAG_ITEM_DESTINATION);
	RegisterTagItemType("TAG / SID-STAR", RG_BREMEN_TAG_ITEM_SIDSTAR);
	RegisterTagItemType("TAG / CFL", RG_BREMEN_TAG_ITEM_CFL);
	RegisterTagItemType("TAG / AHDG", RG_BREMEN_TAG_ITEM_AHDG);
	RegisterTagItemType("TAG / NORDO", RG_BREMEN_TAG_ITEM_NORDO);
	RegisterTagItemType("TAG / COPX", RG_BREMEN_TAG_ITEM_COPX);
	RegisterTagItemType("TAG / XFL", RG_BREMEN_TAG_ITEM_XFL);
	RegisterTagItemType("TAG / NEXT SCT", RG_BREMEN_TAG_ITEM_NEXTSECTOR);


	RegisterTagItemType("LIST / Climb Via Sid", RG_BREMEN_LIST_ITEM_CLBSID);
	RegisterTagItemType("LIST / Descend Via Star", RG_BREMEN_LIST_ITEM_DESCSTAR);

	//RegisterTagItemFunction("TAG FUNC / NORDO SET", RG_BREMEN_TAG_ITEM_FUNC_NORDO_SET);
	//RegisterTagItemFunction("TAG FUNC / NORDO CLEAR", RG_BREMEN_TAG_ITEM_FUNC_NORDO_CLEAR);

	//m_NordoList = RegisterFpList("NORDO List");
	//if (m_NordoList.GetColumnNumber() == 0) {
	//		m_NordoList.AddColumnDefinition("Callsign", 8, false, NULL, TAG_ITEM_TYPE_CALLSIGN, NULL, RG_BREMEN_TAG_ITEM_FUNC_NORDO_CLEAR, NULL, TAG_ITEM_FUNCTION_NO);
	//}
	//m_NordoList.ShowFpList(false);

	/*DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, "Loaded RG Bremen Plugin at version", true, true, false, false, false);
	DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, PLUGIN_VERSION, true, true, false, false, false);
	DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, PLUGIN_DEVELOPER, true, true, false, false, false);*/
}

CRGBremenPlugin::~CRGBremenPlugin()
{
}

void CRGBremenPlugin::OnGetTagItem(EuroScopePlugIn::CFlightPlan FlightPlan, EuroScopePlugIn::CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int * pColorCode, COLORREF * pRGB, double * pFontSize)
{
	if (!FlightPlan.IsValid()) return;

	// Check list items first
	if (ItemCode == RG_BREMEN_LIST_ITEM_CLBSID) {
		// Analyze SID and report to the item
		if (analzyeSid(FlightPlan)) {
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = colorClimbViaSidYes;
			strcpy_s(sItemString, 16, "YES");
		}
		else {
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = colorClimbViaSidNo;
			strcpy_s(sItemString, 16, "NO");
		}
		return;
	}
	if (ItemCode == RG_BREMEN_LIST_ITEM_DESCSTAR) {
		// Analyze SID and report to the item
		if (analzyeStarTrans(FlightPlan)) {
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = colorClimbViaSidYes;
			strcpy_s(sItemString, 16, "YES");
		}
		else {
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = colorClimbViaSidNo;
			strcpy_s(sItemString, 16, "NO");
		}
		return;
	}

	// Check for non rg coloring items
	if (ItemCode == RG_BREMEN_TAG_ITEM_SIDSTAR) {
		// Determine if the aircraft is departing or approaching

		// TODO: Find a better way for checking this
		if (FlightPlan.GetDistanceToDestination() > 100.0 && FlightPlan.GetDistanceFromOrigin() < FlightPlan.GetDistanceToDestination()) {
			// Analyze SID and report to the item
			if (analzyeSid(FlightPlan)) {
				*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
				*pRGB = RGB(0, 255, 0);
				strcpy_s(sItemString, 16, FlightPlan.GetFlightPlanData().GetSidName());
			}
			else {
				/**pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
				*pRGB = RGB(255, 0, 0);*/
				strcpy_s(sItemString, 16, FlightPlan.GetFlightPlanData().GetSidName());
			}
		}
		else {
			// Analyze STAR and report to the item
			if (analzyeStarTrans(FlightPlan)) {
				*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
				*pRGB = RGB(0, 255, 0);
				strcpy_s(sItemString, 16, FlightPlan.GetFlightPlanData().GetStarName());
			}
			else {
				/**pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
				*pRGB = RGB(255, 0, 0);*/
				strcpy_s(sItemString, 16, FlightPlan.GetFlightPlanData().GetStarName());
			}
		}
		
		return;
	}

	// Set tag item color
	if (isVFR(atoi(RadarTarget.GetPosition().GetSquawk()))) {
		*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
		*pRGB = colorVfr;
	}

	// Needed vars
	int vs = RadarTarget.GetVerticalSpeed() / 200;
	int fl = RadarTarget.GetPosition().GetFlightLevel();
	int cfl = FlightPlan.GetControllerAssignedData().GetClearedAltitude();
	int hdg = FlightPlan.GetControllerAssignedData().GetAssignedHeading();
	string dct = FlightPlan.GetControllerAssignedData().GetDirectToPointName();
	int idxNordoData;
	const char* copxPointName;
	const char* xfl;
	const char* nextSector;

	switch (ItemCode)
	{
	case RG_BREMEN_TAG_ITEM_VFR_INDICATOR:
		if (FlightPlan.GetState() == EuroScopePlugIn::FLIGHT_PLAN_STATE_TRANSFER_FROM_ME_INITIATED || FlightPlan.GetState() == EuroScopePlugIn::FLIGHT_PLAN_STATE_TRANSFER_TO_ME_INITIATED) {
			// Override handoff color
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = colorHandoff;
		}
		// If the target is a vfr target display only the VFR indicator
		if (isVFR(atoi(RadarTarget.GetPosition().GetSquawk()))) {
			switch (atoi(RadarTarget.GetPosition().GetSquawk()))
			{
			case 0:
				strncpy(sItemString, "0000\0", 15); break;
			case 20:
				strncpy(sItemString, "RESCU\0", 15); break;
			case 23:
				strncpy(sItemString, "BPO\0", 15); break;
			case 24:
				strncpy(sItemString, "TFFN\0", 15); break;
			case 25:
				strncpy(sItemString, "PJE\0", 15); break;
			case 27:
				strncpy(sItemString, "ACRO\0", 15); break;
			case 30:
				strncpy(sItemString, "CAL\0", 15); break;
			case 31:
				strncpy(sItemString, "OPSKY\0", 15); break;
			case 33:
				strncpy(sItemString, "VM\0", 15); break;
			case 34:
				strncpy(sItemString, "SAR\0", 15); break;
			case 35:
				strncpy(sItemString, "AIRCL\0", 15); break;
			case 36:
				strncpy(sItemString, "POL\0", 15); break;
			case 37:
				strncpy(sItemString, "BIV\0", 15); break;
			case 76:
				strncpy(sItemString, "VFRCD\0", 15); break;
			case 1200:
				strncpy(sItemString, "1200\0", 15); break;
			case 2000:
				strncpy(sItemString, "2000\0", 15); break;
			case 2200:
				strncpy(sItemString, "2200\0", 15); break;
			case 7000:
				strncpy(sItemString, "V\0", 15); break;
			case 7740:
				strncpy(sItemString, "FIS\0", 15); break;
			case 7741:
				strncpy(sItemString, "FIS\0", 15); break;
			case 7742:
				strncpy(sItemString, "FIS\0", 15); break;
			case 7743:
				strncpy(sItemString, "FIS\0", 15); break;
			case 7744:
				strncpy(sItemString, "FIS\0", 15); break;
			case 7745:
				strncpy(sItemString, "FIS\0", 15); break;
			default:
				strncpy(sItemString, FlightPlan.GetCallsign() + '\0', 15);  break;
			}
		}
		else {
			switch (atoi(RadarTarget.GetPosition().GetSquawk()))
			{
			case 1200:
				strncpy(sItemString, "1200\0", 15); break;
			case 2000:
				strncpy(sItemString, "2000\0", 15); break;
			case 2200:
				strncpy(sItemString, "2200\0", 15); break;
			default:
				strncpy(sItemString, RadarTarget.GetCallsign() + '\0', 15); break;
			}

		}
		break;
	case RG_BREMEN_TAG_ITEM_GROUNDSPEED:
		if (RadarTarget.GetGS() / 10 <= 99) {
			_itoa(RadarTarget.GetGS() / 10, sItemString, 10);
			strcat(sItemString, "\0");
		}
		else {
			strncpy(sItemString, "   \0", 15);
		}
		break;
	case RG_BREMEN_TAG_ITEM_VFR_CALLSIGN:
		if (isVFR(atoi(RadarTarget.GetPosition().GetSquawk()))) {
			strncpy(sItemString, RadarTarget.GetCallsign(), 15);
		}
		else {
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = colorVfr;
			switch (atoi(RadarTarget.GetPosition().GetSquawk()))
			{
			case 1200:
				strncpy(sItemString, RadarTarget.GetCallsign() + '\0', 15); break;
			case 2000:
				strncpy(sItemString, RadarTarget.GetCallsign() + '\0', 15); break;
			case 2200:
				strncpy(sItemString, RadarTarget.GetCallsign() + '\0', 15); break;
			default:
				strncpy(sItemString, RadarTarget.GetCallsign() + '\0', 15); break;
			}
		}
		break;
	case RG_BREMEN_TAG_ITEM_TWR_CALLSIGN:
		// Override callsign color
		*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
		*pRGB = colorTwr;
		// If the target is a vfr target display only the VFR indicator
		if (isVFR(atoi(RadarTarget.GetPosition().GetSquawk()))) {
			switch (atoi(RadarTarget.GetPosition().GetSquawk()))
			{
			case 0:
				strncpy(sItemString, "0000\0", 15); break;
			case 20:
				strncpy(sItemString, "RESCU\0", 15); break;
			case 23:
				strncpy(sItemString, "BPO\0", 15); break;
			case 24:
				strncpy(sItemString, "TFFN\0", 15); break;
			case 25:
				strncpy(sItemString, "PJE\0", 15); break;
			case 27:
				strncpy(sItemString, "ACRO\0", 15); break;
			case 30:
				strncpy(sItemString, "CAL\0", 15); break;
			case 31:
				strncpy(sItemString, "OPSKY\0", 15); break;
			case 33:
				strncpy(sItemString, "VM\0", 15); break;
			case 34:
				strncpy(sItemString, "SAR\0", 15); break;
			case 35:
				strncpy(sItemString, "AIRCL\0", 15); break;
			case 36:
				strncpy(sItemString, "POL\0", 15); break;
			case 37:
				strncpy(sItemString, "BIV\0", 15); break;
			case 76:
				strncpy(sItemString, "VFRCD\0", 15); break;
			case 1200:
				strncpy(sItemString, "1200\0", 15); break;
			case 2000:
				strncpy(sItemString, "2000\0", 15); break;
			case 2200:
				strncpy(sItemString, "2200\0", 15); break;
			case 7000:
				strncpy(sItemString, "V\0", 15); break;
			case 7740:
				strncpy(sItemString, "FIS\0", 15); break;
			case 7741:
				strncpy(sItemString, "FIS\0", 15); break;
			case 7742:
				strncpy(sItemString, "FIS\0", 15); break;
			case 7743:
				strncpy(sItemString, "FIS\0", 15); break;
			case 7744:
				strncpy(sItemString, "FIS\0", 15); break;
			case 7745:
				strncpy(sItemString, "FIS\0", 15); break;
			default:
				strncpy(sItemString, FlightPlan.GetCallsign() + '\0', 15);  break;
			}
		}
		else {
			switch (atoi(RadarTarget.GetPosition().GetSquawk()))
			{
			case 1200:
				strncpy(sItemString, "1200\0", 15); break;
			case 2000:
				strncpy(sItemString, "2000\0", 15); break;
			case 2200:
				strncpy(sItemString, "2200\0", 15); break;
			default:
				strncpy(sItemString, RadarTarget.GetCallsign() + '\0', 15); break;
			}

		}
		break;
	case RG_BREMEN_TAG_ITEM_ALTITUDE:
		*pColorCode = EuroScopePlugIn::TAG_COLOR_DEFAULT;
		if (fl <= GetTransitionAltitude()) {
			fl = RadarTarget.GetPosition().GetPressureAltitude();
			strncpy(sItemString, ('A' + padWithZeros(4, fl)).substr(0, 3).c_str(), 15);
		}
		else {
			strncpy(sItemString, padWithZeros(5, fl).substr(0, 3).c_str(), 15);
		}
		break;
	case RG_BREMEN_TAG_ITEM_VS_INDICATOR:
		if (!isVFR(atoi(RadarTarget.GetPosition().GetSquawk()))) {
			if (vs < -2) {
				strncpy(sItemString, "|\0", 15);
			}
			if (vs > 2) {
				strncpy(sItemString, "^\0", 15);
			}
		}
		break;
	case RG_BREMEN_TAG_ITEM_VS_SPEED:
		if (!isVFR(atoi(RadarTarget.GetPosition().GetSquawk()))) {
			vs = (vs < 0) ? vs * -1 : vs;
			if (vs <= 2) {
				strncpy(sItemString, "\0", 15);
			}
			else if (vs < 10) {
				char* tmp = new char[15];
				_itoa(vs, tmp, 10);
				sItemString[0] = '0';
				sItemString[1] = tmp[0];
				sItemString[2] = '\0';
			}
			else {
				_itoa(vs, sItemString, 10);
			}
		}
		break;
	case RG_BREMEN_TAG_ITEM_IAS:
		if (!isVFR(atoi(RadarTarget.GetPosition().GetSquawk()))) {
			if (RadarTarget.GetPosition().GetReportedGS() / 10 <= 99) {
				char* tmp = new char[15];
				_itoa(RadarTarget.GetPosition().GetReportedGS() / 10, tmp, 10);
				sItemString[0] = 'N';
				sItemString[1] = tmp[0];
				sItemString[2] = tmp[1];
				sItemString[3] = '\0';
			}
			else {
				strncpy(sItemString, "   \0", 15);
			}
		}
		else {
			strncpy(sItemString, "   \0", 15);
		}
		break;
	case RG_BREMEN_TAG_ITEM_DESTINATION:
		if (!isVFR(atoi(RadarTarget.GetPosition().GetSquawk()))) {
			strncpy(sItemString, FlightPlan.GetFlightPlanData().GetDestination(), 15);
		}
		else {
			strncpy(sItemString, "   \0", 15);
		}
		break;
	case RG_BREMEN_TAG_ITEM_CFL:
		if (!isVFR(atoi(RadarTarget.GetPosition().GetSquawk())) && FlightPlan.GetFlightPlanData().GetPlanType()[0] == 'I') {

			if (cfl == 0) cfl = FlightPlan.GetFinalAltitude();

			if (cfl == 1) strcpy_s(sItemString, 15, "APP");
			else if (cfl == 2) strcpy_s(sItemString, 15, "VIS");
			else if (cfl > 9999) {
				_itoa(cfl / 100, sItemString, 10);
			}
			else if (cfl > 999) {
				char* tmp = new char[5];
				_itoa(cfl / 100, tmp, 10);
				sItemString[0] = '0';
				sItemString[1] = tmp[0];
				sItemString[2] = tmp[1];
				sItemString[3] = '\0';
			}
			else if (cfl > 99) {
				char* tmp = new char[5];
				_itoa(cfl / 100, tmp, 10);
				sItemString[0] = '0';
				sItemString[1] = '0';
				sItemString[2] = tmp[0];
				sItemString[3] = '\0';
			}
			else {
				strcpy_s(sItemString, 15, "000");
			}
		}
		break;
	case RG_BREMEN_TAG_ITEM_AHDG:
		if (!isVFR(atoi(RadarTarget.GetPosition().GetSquawk())) && FlightPlan.GetFlightPlanData().GetPlanType()[0] == 'I') {
			if (dct != "") {
				strcpy_s(sItemString, 15, dct.c_str());
			}
			else if (hdg != 0) {
				string hdgStr = "H" + padWithZeros(3, hdg);
				strcpy_s(sItemString, 15, hdgStr.c_str());
			}
			else
				strncpy(sItemString, "\0", 15);
		}
		break;

	/*case RG_BREMEN_TAG_ITEM_NORDO:
		idxNordoData = getNordoFlightPlanIndex(FlightPlan.GetCallsign());
		if (idxNordoData >= 0) {
			strcpy_s(sItemString, 15, "NORDO");
		}
		else {
			strcpy_s(sItemString, 15, "ON FREQ");
		}
		break;*/
	case RG_BREMEN_TAG_ITEM_COPX:
		copxPointName = GetCopxPointName(FlightPlan);
		strcpy_s(sItemString, 15, copxPointName);
		break;
	case RG_BREMEN_TAG_ITEM_XFL:
		break;
	case RG_BREMEN_TAG_ITEM_NEXTSECTOR:
		break;
	default:
		break;
	}
}

bool CRGBremenPlugin::OnCompileCommand(const char * sCommandLine)
{
	regex regex("[\\s]+");
	vector<string> sCommandLineSplit = Tokenize(sCommandLine, regex);

	//DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, "Command Input: ", true, false, false, false, false);
	for (int i = 0; i < sCommandLineSplit.size(); i++) {
		DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, sCommandLineSplit.at(i).c_str(), true, false, false, false, false);
	}

	if (sCommandLineSplit.at(0) == ".rgbremen") {
		// RG Bremen PlugIn Command found
		if (sCommandLineSplit.at(1) == "reload") {
			loadConfiguration();
			readSidStarRestrictionsList();
			DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, "Plugin configuration reloaded", true, true, false, false, false);
			return true;
		}
		/*if (sCommandLineSplit.at(1) == "nordo") {
			if (sCommandLineSplit.at(2) == "show") {
				m_NordoList.ShowFpList(true);
				DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, "NORDO List shown", true, true, false, false, false);
				return true;
			}
			if (sCommandLineSplit.at(2) == "hide") {
				m_NordoList.ShowFpList(false);
				DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, "NORDO List hidden", true, true, false, false, false);
				return true;
			}
			if (sCommandLineSplit.at(2) == "report") {
				try {
					DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, "Generating NORDO report file....", true, true, false, false, false);

					string nordoFilePath;

					nordoFilePath = getCurrentDirectory() + "\\NORDO_REPORT_" + time_to_string() + ".txt";

					WriteNordoReport(nordoFilePath);

					DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, "NORDO report written to:", true, true, false, false, false);
					DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, nordoFilePath.c_str(), true, true, false, false, false);
				}
				catch (...) {
					DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, "Failed to write NORDO report.", true, true, false, false, false);
				}
				return true;
			}
		}*/
		if (sCommandLineSplit.at(1) == "scenario") {
			if (sCommandLineSplit.size() != 5) {
				DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, "Scenario File: .rgbremen scenario <name> <icao1,icao2,...,icaoN> <initialPseudo>", true, true, false, false, false);
				return true;
			}

			string scenarioName = sCommandLineSplit.at(2);
			string airports = sCommandLineSplit.at(3);
			string initialPseudo = sCommandLineSplit.at(4);
			string postfields = "name=" + scenarioName + "&icao=" + airports + "&range=500&maxFlights=1000&depArrScale=50&depAltLimit=47000&minSquawk=1000&maxSquawk=7777&initialPseudo=" + initialPseudo;
			DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, postfields.c_str(), true, true, false, false, false);

			try
			{
				CURL* curl;

				curl_global_init(CURL_GLOBAL_ALL);

				struct curl_slist* list = NULL;

				curl = curl_easy_init();
				if (curl) {
					string authHeader = "rgbremen-authentication: ";
					authHeader.append(RG_BREMEN_SCENARIO_CREATOR_KEY);

					list = curl_slist_append(list, authHeader.c_str());

					string readBuffer;

					curl_easy_setopt(curl, CURLOPT_URL, RG_BREMEN_SCENARIO_CREATOR_URL);
					curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
					curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
					//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
					curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
					curl_easy_setopt(curl, CURLOPT_POST, 1L);
					curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.116 Safari/537.36");
					//curl_easy_setopt(curl, CURLOPT_REFERER, RG_BREMEN_SCENARIO_CREATOR_URL);
					curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields);
					curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
					curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

					CURLcode res = curl_easy_perform(curl);

					if (res != CURLE_OK) {
						DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, curl_easy_strerror(res), true, true, false, false, false);
					}
					else {
						DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, "Scenario created. You can download it from https://dev.vatsim-germany.org/euroscope/simsession", true, true, false, false, false);
						DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, readBuffer.c_str(), true, true, false, false, false);
					}

					curl_easy_cleanup(curl);
					curl_slist_free_all(list);
				}
				curl_global_cleanup();

			}
			catch (const std::exception& e)
			{
				DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, e.what(), true, true, false, false, false);
			}

			return true;
		}
	}

	// Always return false
	return false;
}

void CRGBremenPlugin::WriteNordoReport(std::string& nordoFilePath)
{
	ofstream outfile(nordoFilePath);

	outfile << "Callsign | Controller Callsign | Communication Type | Pilot Name | Origin | Route | Destination | Remarks" << endl;

	for (int i = 0; i < m_NordoData.GetSize(); i++)
	{
		outfile << m_NordoData[i].GetCallsign() << "|";
		outfile << m_NordoData[i].GetTrackingControllerCallsign() << "|";
		outfile << m_NordoData[i].GetFlightPlanData().GetCommunicationType() << "|";
		//outfile << m_NordoData[i].GetPilotName() << "|"; // Due to DSGVO
		outfile << "Anonymous" << "|";
		outfile << m_NordoData[i].GetFlightPlanData().GetOrigin() << "|";
		outfile << m_NordoData[i].GetFlightPlanData().GetRoute() << "|";
		outfile << m_NordoData[i].GetFlightPlanData().GetDestination() << "|";
		outfile << m_NordoData[i].GetFlightPlanData().GetRemarks() << endl;
	}

	outfile.close();
}

void CRGBremenPlugin::OnFunctionCall(int FunctionId, const char* sItemString, POINT Pt, RECT Area)
{
	CFlightPlan fp = FlightPlanSelectASEL();
	if (!fp.IsValid()) return;

	int idxNordoData;

	switch (FunctionId)
	{
	case RG_BREMEN_TAG_ITEM_FUNC_NORDO_CLEAR:
		m_NordoList.RemoveFpFromTheList(fp);
		idxNordoData = getNordoFlightPlanIndex(fp.GetCallsign());
		if (idxNordoData >= 0) {
			m_NordoData.RemoveAt(idxNordoData);
		}
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_NORDO_SET:
		m_NordoList.AddFpToTheList(fp);
		idxNordoData = getNordoFlightPlanIndex(fp.GetCallsign());
		if (idxNordoData >= 0) {
			m_NordoData.RemoveAt(idxNordoData);
		}
		m_NordoData.Add(fp);
		//WriteNordoReport(getCurrentDirectory() + "\\NORDO_REPORT_" + time_to_string() + ".txt");
		break;
	default:
		break;
	}
}

const char* CRGBremenPlugin::GetCopxPointName(EuroScopePlugIn::CFlightPlan FlightPlan)
{
	const char* copx = "";

	if (FlightPlan.GetTrackingControllerIsMe()) {
		const char* flNCPN = FlightPlan.GetNextCopxPointName();
		EuroScopePlugIn::CFlightPlanExtractedRoute flightPlanRoute = FlightPlan.GetExtractedRoute();
		
		if (flightPlanRoute.GetPointsAssignedIndex() >= 0) {
			if (flNCPN != flightPlanRoute.GetPointName(flightPlanRoute.GetPointsAssignedIndex())) {
				copx = "COORD";
			}
		}
		else {
			EuroScopePlugIn::CFlightPlanPositionPredictions flightPlanPredictions = FlightPlan.GetPositionPredictions();
			for (int i = 0; i < flightPlanPredictions.GetPointsNumber(); i++) {
				if (flightPlanPredictions.GetControllerId(i) != this->ControllerMyself().GetPositionId()) {
					copx = flightPlanRoute.GetPointName(i);
					break;
				}
			}
		}
		
	}

	return copx;
}

string CRGBremenPlugin::padWithZeros(int padding, int s)
{
	stringstream ss;
	ss << setfill('0') << setw(padding) << s;
	return ss.str();
}

int CRGBremenPlugin::getNordoFlightPlanIndex(const char* sCallsign)
{
	int i;
	for (i = 0; i < m_NordoData.GetSize(); i++) {
		if (m_NordoData[i].GetCallsign() == sCallsign) {
			if (i < m_NordoData.GetSize()) return i;
		}
	}
	return -1;
}

string CRGBremenPlugin::time_to_string()
{
	stringstream sstr;
	time_t t = time(NULL);
	sstr << t;
	return sstr.str();
}