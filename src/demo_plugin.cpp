// Copyright(C) 2026 by Steven Adler
//
// This file is part of the Demo plugin for OpenCPN.
//
// Demo plugin for OpenCPN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Demo plugin for OpenCPN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Demo plugin for OpenCPN. If not, see <https://www.gnu.org/licenses/>.
//

//
// Project: Gateway Plugin
// Description: Derived from Demo plugin, a simple bi-directional NMEA 0183 <-> NMEA 2000 gateway
// Owner: twocanplugin@hotmail.com
// Date: 17/04/2026
// Version History: 
// 1.0 Initial Release

#include "demo_plugin.h"

#include "demo_gateway_ui.h"

#include "demo_globals.h"

// The class factories, used to create and destroy instances of the PlugIn
extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr) {
	return new DemoPlugin(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) {
	delete p;
}

// Constructor
// As the plugin uses newer functions, it requires a plugin API version of at least 1.20
DemoPlugin::DemoPlugin(void* ppimgr) : opencpn_plugin_120(ppimgr), wxEvtHandler() {
	
	// Initialize the plugin bitmap, converting from SVG to PNG. Refer to GetPluginBitmap below
	// Note the icon file is located in the source repository data folder
	// and consequently will be installed into user's plugin data fiolder
	wxString pluginFolder = GetPluginDataDir(PKG_NAME) + wxFileName::GetPathSeparator() + "data" + wxFileName::GetPathSeparator();
	g_pluginBitmap = GetBitmapFromSVGFile(pluginFolder + "demo_plugin.svg", 32, 32);
}

// Destructor
DemoPlugin::~DemoPlugin(void) {
}

// Perform plugin initialization here. At this point most of OpenCPN has been initialised and most of the plugin API's can be invoked
int DemoPlugin::Init(void) {

	// Dump some of OpenCPN's special folder paths
	wxLogMessage("Demo Plugin, OpenCPN Program Path (opencpn executable): %s", GetOCPN_ExePath());
	wxLogMessage("Demo Plugin, OpenCPN Plugin Path (built-in plugins): %s", *GetpPlugInLocation());
	wxLogMessage("Demo Plugin, OpenCPN Private Data Path (logs, config): %s", *GetpPrivateApplicationDataLocation());
	wxLogMessage("Demo Plugin, OpenCPN Shared Data Path: %s", *GetpSharedDataLocation());
	wxLogMessage("Demo Plugin, Any Plugin's Writeable Documents Path: %s", GetWritableDocumentsDir());
	wxLogMessage("Demo Plugin, This Plugin's Data Path: %s", GetPluginDataDir(PKG_NAME));
	wxLogMessage("Demo Plugin, This Plugin's Library Path: %s", GetPlugInPath(this));

	// Dump some of OpenCPN's user measurement preferences
	wxLogMessage("Demo Plugin, Temperature Units: %s", getUsrTempUnit_Plugin());
	wxLogMessage("Demo Plugin, Speed Units: %s", getUsrSpeedUnit_Plugin());

	// Dump the location of the user's documents folder
	wxLogMessage("Demo Plugin, User's Documents: %s", wxStandardPaths::Get().GetDocumentsDir());
	
	// Load the previously saved settings
	LoadSettings();

	// Notify OpenCPN what callbacks the plugin registers to receive
	return (WANTS_CONFIG | INSTALLS_TOOLBOX_PAGE | WANTS_PREFERENCES | WANTS_LATE_INIT);
}

// OpenCPN is either closing down, or the plugin has been disabled from the Preferences Dialog
bool DemoPlugin::DeInit(void) {

	// Note, OpenCPN doesn't actually care about the return value
	return true; 
}

// Unnecessary to use Late Initialization in this example, however in case a plugin is loaded 
// before OpenCPN core services have been started, it allows a plugin to perform further 
// initialization. Requires WANTS_LATE_INIT
void DemoPlugin::LateInit(void) {

	// Register subscriber for NMEA 0183 VHW Speed sentence
	wxDEFINE_EVENT(EVT_183_VHW, ObservedEvt);
	NMEA0183Id id_vhw = NMEA0183Id("VHW");
	listener_vhw = std::move(GetListener(id_vhw, EVT_183_VHW, this));
	Bind(EVT_183_VHW, [&](ObservedEvt ev) {
		ParseVHW(ev);
		});

	// Register subscriber for NMEA 0183 DPT Depth sentence
	wxDEFINE_EVENT(EVT_183_DPT, ObservedEvt);
	NMEA0183Id id_dpt = NMEA0183Id("DPT");
	listener_dpt = std::move(GetListener(id_dpt, EVT_183_DPT, this));
	Bind(EVT_183_DPT, [&](ObservedEvt ev) {
		ParseDPT(ev);
		});

	// Register subscriber for NMEA 0183 MWV Wind sentence
	wxDEFINE_EVENT(EVT_183_MWV, ObservedEvt);
	NMEA0183Id id_mwv = NMEA0183Id("MWV");
	listener_mwv = std::move(GetListener(id_mwv, EVT_183_MWV, this));
	Bind(EVT_183_MWV, [&](ObservedEvt ev) {
		ParseMWV(ev);
		});

	// Register subscriber for NMEA 0183 GLL Position sentence
	wxDEFINE_EVENT(EVT_183_GLL, ObservedEvt);
	NMEA0183Id id_gll = NMEA0183Id("GLL");
	listener_gll = std::move(GetListener(id_gll, EVT_183_GLL, this));
	Bind(EVT_183_GLL, [&](ObservedEvt ev) {
		ParseGLL(ev);
		});

	// Register subscriber for PGN 130306 Wind
	wxDEFINE_EVENT(EVT_N2K_130306, ObservedEvt);
	NMEA2000Id id_130306 = NMEA2000Id(130306);
	listener_130306 = std::move(GetListener(id_130306, EVT_N2K_130306, this));
	Bind(EVT_N2K_130306, [&](ObservedEvt ev) {
		HandleN2K_130306(ev);
		});

	// Register subscriber for PGN 128267 Depth
	wxDEFINE_EVENT(EVT_N2K_128267, ObservedEvt);
	NMEA2000Id id_128267 = NMEA2000Id(128267);
	listener_128267 = std::move(GetListener(id_128267, EVT_N2K_128267, this));
	Bind(EVT_N2K_128267, [&](ObservedEvt ev) {
		HandleN2K_128267(ev);
		});

	// Register Subcriber for PGN 128259 Boat Speed
	wxDEFINE_EVENT(EVT_N2K_128259, ObservedEvt);
	NMEA2000Id id_128259 = NMEA2000Id(128259);
	listener_128259 = std::move(GetListener(id_128259, EVT_N2K_128259, this));
	Bind(EVT_N2K_128259, [&](ObservedEvt ev) {
		HandleN2K_128259(ev);
		});

	// Register Subcriber for PGN 129025 Position
	wxDEFINE_EVENT(EVT_N2K_129025, ObservedEvt);
	NMEA2000Id id_129025 = NMEA2000Id(129025);
	listener_129025 = std::move(GetListener(id_129025, EVT_N2K_129025, this));
	Bind(EVT_N2K_129025, [&](ObservedEvt ev) {
		HandleN2K_129025(ev);
		});

	// For the NMEA 2000 interface, plugins need to register what NMEA 2000 PGN's they transmit.
	// This is required for Actisense NGT-1 Adapters
	// Presumably results in a null operation (NOP) for other NMEA 2000 adapters
	if (!g_nmea2000Driver.empty()) {
		std::vector<int> transmittedPGN;
		for (const auto& [pgn, description] : supportedConversions) {
			transmittedPGN.push_back(pgn);
		}
		RegisterTXPGNs(g_nmea2000Driver.ToStdString(), transmittedPGN);
	}
}

// OpenCPN Plugin "housekeeping" methods. All plugins MUST implement these

// Indicate what version of the OpenCPN Plugin API is supported
// Note if using the TestPlugin template, replace these with OCPN_API_VERSION_MAJOR 
// and OCPN_API_VERSION_MINOR
int DemoPlugin::GetAPIVersionMajor() {
	return atoi(API_VERSION);
}

int DemoPlugin::GetAPIVersionMinor() {
	std::string v(API_VERSION);
	size_t dotpos = v.find('.');
	return atoi(v.substr(dotpos + 1).c_str());
}

// The plugin version numbers. 
int DemoPlugin::GetPlugInVersionMajor() {
	return PLUGIN_VERSION_MAJOR;
}

int DemoPlugin::GetPlugInVersionMinor() {
	return PLUGIN_VERSION_MINOR;
}

// BUG BUG Need to add patch version to config.h.in
int DemoPlugin::GetPlugInVersionPatch() {
	return 0;
}

// Return descriptions for the Plugin
// Note if using TestPlugin template use PLUGIN_COMMON_NAME
wxString DemoPlugin::GetCommonName() {
	return PLUGIN_API_NAME;
}

// Note if using TestPlugin template use PLUGIN_SHORT_DESCRIPTION
wxString DemoPlugin::GetShortDescription() {
	return PKG_SUMMARY;
}

// Note if using TestPlugin template use PLUGIN_LONG_DESCRIPTION
wxString DemoPlugin::GetLongDescription() {
	return PKG_DESCRIPTION;
}

// Most plugins use a 32x32 pixel PNG file converted to xpm by pgn2wx.pl perl script
// However it might be easier to use a SVG file as you only need to maintain one image format
wxBitmap* DemoPlugin::GetPlugInBitmap() {
	return &g_pluginBitmap;
}

// When the plugin is enabled, this API provides the opportunity to configure initial settings
void DemoPlugin::SetDefaults(void) {
	auto gatewaySettings = std::make_unique<DemoGatewayUI>(GetOCPNCanvasWindow(), wxID_ANY, _("Demo Preferences"));
	if (wxID_OK == gatewaySettings->ShowModal()) {
		SaveSettings();
	}
}

// This API seems to be deprecated?
void DemoPlugin::SetupToolboxPanel(int page_sel, wxNotebook* pnotebook) {
	wxMessageBox(wxString::Format("SetupToolboxPanel invoked: %d", page_sel), "Demo Plugin");
}

// Invoked from he plugin's preferences option, enabling the user to modify the plugin's settings.
// Requires WANTS_PREFERENCES
void DemoPlugin::ShowPreferencesDialog(wxWindow* parent) {
	auto gatewaySettings = std::make_unique<DemoGatewayUI>(parent, wxID_ANY, _("Demo Preferences"));
	if (wxID_OK == gatewaySettings->ShowModal()) {
		SaveSettings();
	}
}

// Parse NMEA 0183 Speed through Waterl
void DemoPlugin::ParseVHW(ObservedEvt ev) {
	if (g_Speed == ConversionType::NMEA2000) {
		NMEA0183Id id_183_vhw("VHW");
		NMEA0183 parserNMEA0183;
		wxString sentence = GetN0183Payload(id_183_vhw, ev);
		parserNMEA0183 << sentence;

		if (parserNMEA0183.Parse()) {
			auto boatSpeed = parserNMEA0183.Vhw.Knots;
			auto payload = GeneratePGN128259(toUsrSpeed_Plugin(boatSpeed, 3));
			SendNMEA2000(g_nmea2000Driver.ToStdString(), 255, 4, 128259, payload);
		}
	}
}

// Handle NMEA 0183 Wind Speed
void DemoPlugin::ParseMWV(ObservedEvt ev) {
	if (g_Wind == ConversionType::NMEA2000) {
		NMEA0183Id id_183_mwv("MWV");
		NMEA0183 parserNMEA0183;
		wxString sentence = GetN0183Payload(id_183_mwv, ev);
		parserNMEA0183 << sentence;

		if (parserNMEA0183.Parse()) {
			double windSpeed = parserNMEA0183.Mwv.WindSpeed;
			double windAngle = parserNMEA0183.Mwv.WindAngle;
			wxString units = parserNMEA0183.Mwv.WindSpeedUnits;
			wxString reference = parserNMEA0183.Mwv.Reference;


			// Only interested in apparent wind angle rather than true wind angle
			if (reference == "R") {
				windAngle = parserNMEA0183.Mwv.WindAngle * M_PI / 180;
				// Using the helper API fromUsrWindSpeed_Plugin to convert the speed into knots
				// WSPEED_KTS = 0, WSPEED_MS, WSPEED_MPH, WSPEED_KMH
				if (units == "N") {
					// Knots (Nautical Miles per Hour)
					windSpeed = toUsrWindSpeed_Plugin(windSpeed, 1);
				}
				else if (units == "M") {
					// Metres per second, No conversion required
				}
				else if (units == "S") {
					// Statute Miles per hour
					windSpeed = windSpeed * 0.44704;
				}
				else if (units == "K") {
					// Kilometres per hour
					windSpeed = windSpeed / 3.6;
				}
				auto payload = GeneratePGN130306(windSpeed, windAngle);
				SendNMEA2000(g_nmea2000Driver.ToStdString(), 255, 4, 130306, payload);
			}
		}
	}
}

void DemoPlugin::ParseDPT(ObservedEvt ev) {
	if (g_Depth == ConversionType::NMEA2000) {
		NMEA0183Id id_183_dpt("DPT");
		NMEA0183 parserNMEA0183;
		wxString sentence = GetN0183Payload(id_183_dpt, ev);
		parserNMEA0183 << sentence;

		if (parserNMEA0183.Parse()) {
			auto depth = parserNMEA0183.Dpt.DepthMeters;
			auto offset = parserNMEA0183.Dpt.OffsetFromTransducerMeters;
			auto payload = GeneratePGN128267(depth, offset);
			SendNMEA2000(g_nmea2000Driver.ToStdString(), 255, 4, 128267, payload);
		}
	}
}

void DemoPlugin::ParseGLL(ObservedEvt ev) {
	if (g_Position == ConversionType::NMEA2000) {
		NMEA0183Id id_183_gll("GLL");
		NMEA0183 parserNMEA0183;
		wxString sentence = GetN0183Payload(id_183_gll, ev);
		parserNMEA0183 << sentence;

		if (parserNMEA0183.Parse()) {
			double latitude = parserNMEA0183.Gll.Position.Latitude.Latitude;
			if (parserNMEA0183.Gll.Position.Latitude.Northing == NORTHSOUTH::South) {
				latitude = -latitude;
			}
			double longitude = parserNMEA0183.Gll.Position.Longitude.Longitude;
			if (parserNMEA0183.Gll.Position.Longitude.Easting == EASTWEST::West) {
				latitude = -latitude;
			}
			// BUG BUG Is the library OK ??
			auto payload = GeneratePGN129025(latitude / 100.0, longitude / 100.0);
			SendNMEA2000(g_nmea2000Driver.ToStdString(), 255, 4, 129025, payload);
		}
	}
}


// Generate NMEA 0183 MWV Sentence 
wxString DemoPlugin::GenerateMWV(double windAngle, double windSpeed) {
	NMEA0183 NMEA0183parser;
	SENTENCE NMEASentence;

	NMEA0183parser.Mwv.Empty();
	NMEA0183parser.TalkerID = "II";
	NMEA0183parser.Mwv.WindAngle = windAngle;
	NMEA0183parser.Mwv.Reference = "R";
	NMEA0183parser.Mwv.WindSpeed = windSpeed;
	NMEA0183parser.Mwv.WindSpeedUnits = "N";
	NMEA0183parser.Mwv.IsDataValid = NTrue;

	NMEA0183parser.Mwv.Write(NMEASentence);

	return NMEASentence.Sentence;
}

// Generate NMEA 0183 Depth Sentence 
wxString DemoPlugin::GenerateDPT(double depth, double offset) {
	NMEA0183 NMEA0183parser;
	SENTENCE NMEASentence;

	NMEA0183parser.Dpt.Empty();
	NMEA0183parser.TalkerID = "II";
	NMEA0183parser.Dpt.DepthMeters = depth;
	NMEA0183parser.Dpt.OffsetFromTransducerMeters = offset;
	NMEA0183parser.Dpt.Write(NMEASentence);
	return NMEASentence.Sentence;
}

wxString DemoPlugin::GenerateGLL(double latitude, double longitude) {
	NMEA0183 NMEA0183parser;
	SENTENCE NMEASentence;

	NMEA0183parser.Gll.Empty();
	NMEA0183parser.TalkerID = "II";
	NMEA0183parser.Gll.Position.Latitude.Latitude = std::fabs(latitude);
	latitude < 0 ? NMEA0183parser.Gll.Position.Latitude.Northing = NORTHSOUTH::South :
		NMEA0183parser.Gll.Position.Latitude.Northing = NORTHSOUTH::North;
	NMEA0183parser.Gll.Position.Longitude.Longitude = std::fabs(longitude);
	longitude < 0 ? NMEA0183parser.Gll.Position.Longitude.Easting = EASTWEST::West :
		NMEA0183parser.Gll.Position.Longitude.Easting = EASTWEST::East;
	NMEA0183parser.Gll.IsDataValid = NTrue;
	NMEA0183parser.Gll.Write(NMEASentence);

	return NMEASentence.Sentence;

}

// Generate Speed Through Water
wxString DemoPlugin::GenerateVHW(double speed) {
	NMEA0183 NMEA0183parser;
	SENTENCE NMEASentence;

	NMEA0183parser.Vhw.Empty();
	NMEA0183parser.TalkerID = "II";
	NMEA0183parser.Vhw.Knots = speed;
	NMEA0183parser.Vhw.Write(NMEASentence);
	return NMEASentence.Sentence;
}

// Send NMEA 0183 Sentence using observer/listener model
void DemoPlugin::SendNMEA0183(const std::string& driverHandle, const std::string& sentence) {
	std::vector<uint8_t> payload(sentence.begin(), sentence.end());

	auto sharedPointer = std::make_shared<std::vector<uint8_t> >(std::move(payload));
	CommDriverResult result = WriteCommDriver(driverHandle, sharedPointer);
	if (result != RESULT_COMM_NO_ERROR) {
		wxLogMessage("Demo Plugin, Error sending NMEA 0183 Sentence: %d", result);
	}
}

// Note for NMEA 2000 data. 
// The payload is not "a NMEA 2000 payload" but an Actisense NGT-1 payload (Don't ask!)
// 
// The text representation of the binary payload once the Data Link Escape (DLE), 
// Start of Transmission (STX) and End of Transmission (ETX) delimiters have been removed is:
// 93 13 02 01 F8 01 FF 01 76 C2 52 00 08 08 70 EB 14 E8 8E 52 D2 BB 10
// This decodes as:
// command (1 byte)		0x93 Value = 0x93 indicates NGT-1 format
// length (1 byte)	    0x13 Length of frame excluding command, length and CRC
// priority (1 byte)    0x02
// PGN (3 bytes)        0x01 0xF8 0x01
// destination(1 byte)  0xFF
// source (1 byte)      0x01
// timestamp (4 bytes)  0x76 0xC2 0x52 0x00
// data length (1 byte) 0x08
// data (data length)   08 70 EB 14 E8 8E 52 D2
// checksum (1 byte)	BB Ensures sum of characters % 256 equals 0
// However the bundled NMEA 2000 parsers handle the "unpacking"


// Parse NMEA 2000 PGN 128269 message (Boat Speed)
void DemoPlugin::HandleN2K_128259(ObservedEvt ev) {
	if (g_Speed == ConversionType::NMEA0183) {
		NMEA2000Id id_128259(128259);
		std::vector<uint8_t> payload = GetN2000Payload(id_128259, ev);
		unsigned char sequenceId;
		double boatSpeedWaterReferenced;
		double boatSpeedGroundReferenced;
		tN2kSpeedWaterReferenceType waterReferenceType;

		if (ParseN2kPGN128259(payload, sequenceId, boatSpeedWaterReferenced, boatSpeedGroundReferenced, waterReferenceType)) {
			// Convert from NMEA 2000 SI units which are m/s to OpenCPN's core units
			wxString sentence = GenerateVHW(fromUsrSpeed_Plugin(boatSpeedWaterReferenced, 3));
			SendNMEA0183(g_nmea0183Driver.ToStdString(), sentence.ToStdString());
		}
	}
}

// Parse NMEA 2000 PGN 130306 message (Wind Speed & Angle)
void DemoPlugin::HandleN2K_130306(ObservedEvt ev) {
	if (g_Wind == ConversionType::NMEA0183) {
		NMEA2000Id id_130306(130306);
		std::vector<uint8_t> payload = GetN2000Payload(id_130306, ev);
		unsigned char sequenceId;
		double windSpeed;
		double windAngle;
		tN2kWindReference windReferenceType;

		if (ParseN2kPGN130306(payload, sequenceId, windSpeed, windAngle, windReferenceType)) {
			// Convert from SI Units, m/s and radians to Knots and Degrees
			wxString sentence = GenerateMWV(windAngle * 180 / M_PI, fromUsrSpeed_Plugin(windSpeed, 3));
			SendNMEA0183(g_nmea0183Driver.ToStdString(), sentence.ToStdString());
		}
	}
}

// Parse NMEA 2000 PGN 128267 (Depth)
void DemoPlugin::HandleN2K_128267(ObservedEvt ev) {
	if (g_Depth == ConversionType::NMEA0183) {
		NMEA2000Id id_128267(128267);
		std::vector<uint8_t> payload = GetN2000Payload(id_128267, ev);
		unsigned char sid;
		double depthBelowTransducer;
		double offset;
		double range;
		if (ParseN2kPGN128267(payload, sid, depthBelowTransducer, offset, range)) {
			wxString sentence = GenerateDPT(depthBelowTransducer, offset);
			SendNMEA0183(g_nmea0183Driver.ToStdString(), sentence.ToStdString());
		}
	}
}

// Parse NMEA 2000 PGN 129025 (Position)
void DemoPlugin::HandleN2K_129025(ObservedEvt ev) {
	if (g_Position == ConversionType::NMEA0183) {
		NMEA2000Id id_129025(129025);
		std::vector<uint8_t> payload = GetN2000Payload(id_129025, ev);
		double latitude;
		double longitude;
		if (ParseN2kPGN129025(payload, latitude, longitude)) {
			wxString sentence = GenerateGLL(latitude, longitude);
			SendNMEA0183(g_nmea0183Driver.ToStdString(), sentence.ToStdString());
		}
	}
}

// Generate NMEA 2000 PGN 130306 message
std::vector<uint8_t> DemoPlugin::GeneratePGN130306(double windSpeed, double windAngle) {
	tN2kMsg msg130306;
	SetN2kWindSpeed(msg130306, 1, windSpeed, windAngle, tN2kWindReference::N2kWind_Apparent);
	std::vector<uint8_t> data(msg130306.Data, msg130306.Data + msg130306.DataLen);
	return data;
}

// Generate NMEA 2000 PGN 128259 message
std::vector<uint8_t> DemoPlugin::GeneratePGN128259(double waterSpeed) {
	tN2kMsg msg;
	SetN2kBoatSpeed(msg, 1, waterSpeed);
	std::vector<uint8_t> data(msg.Data, msg.Data + msg.DataLen);
	return data;
}

// Generate NMEA 2000 PGN 128267 message
std::vector<uint8_t> DemoPlugin::GeneratePGN128267(double depth, double offset) {
	tN2kMsg msg;
	SetN2kPGN128267(msg,1,depth, offset);
	std::vector<uint8_t> data(msg.Data, msg.Data + msg.DataLen);
	return data;
}

// Generate NMEA 2000 PGN 129025 message
std::vector<uint8_t> DemoPlugin::GeneratePGN129025(double latitude, double longitude) {
	tN2kMsg msg;
	SetN2kPGN129025(msg, latitude, longitude);
	std::vector<uint8_t> data(msg.Data, msg.Data + msg.DataLen);
	return data;
}

// Send NMEA 2000 message
void DemoPlugin::SendNMEA2000(const std::string& driverHandle, const unsigned char& destination,
	const unsigned char& priority, const unsigned int pgn, std::vector<uint8_t>& payload) {
	auto sharedPointer = std::make_shared<std::vector<uint8_t>>(payload);
	CommDriverResult result = WriteCommDriverN2K(driverHandle, pgn, destination, priority, sharedPointer);
	if (result != RESULT_COMM_NO_ERROR) {
		wxLogMessage("Demo Plugin, Error sending NMEA 2000 PGN %d: %d", pgn, result);
	}
}

void DemoPlugin::LoadSettings() {
	wxFileConfig* configSettings = GetOCPNConfigObject();
	if (configSettings) {
		configSettings->SetPath("/PlugIns/GatewayPlugin");
		configSettings->Read("NMEA2000", &g_nmea2000Driver, wxEmptyString);
		configSettings->Read("NMEA0183", &g_nmea0183Driver, wxEmptyString);
		g_Speed = static_cast<ConversionType>(configSettings->ReadLong("Speed", 0));
		g_Depth = static_cast<ConversionType>(configSettings->ReadLong("Depth", 0));
		g_Wind = static_cast<ConversionType>(configSettings->ReadLong("Wind", 0));
		g_Position = static_cast<ConversionType>(configSettings->ReadLong("Position", 0));
	}
}

void DemoPlugin::SaveSettings() {
	wxFileConfig* configSettings = GetOCPNConfigObject();
	if (configSettings) {
		configSettings->SetPath("/PlugIns/GatewayPlugin");
		configSettings->Write("NMEA2000", g_nmea2000Driver);
		configSettings->Write("NMEA0183", g_nmea0183Driver);
		configSettings->Write("Wind", static_cast<int>(g_Wind));
		configSettings->Write("Speed", static_cast<int>(g_Speed));
		configSettings->Write("Depth", static_cast<int>(g_Depth));
		configSettings->Write("Position", static_cast<int>(g_Position));
	}
}