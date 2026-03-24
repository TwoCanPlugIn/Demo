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
// Project: Demo Plugin
// Description: Demonstrate the use of the OpenCPN plugin API's
// Owner: twocanplugin@hotmail.com
// Date: 10/01/2026
// Version History: 
// 1.0 Initial Release
// Chapter 1. A Basic plugin, that does little except to dump some common OpenCPN file paths
// Chapter 2. Plugin initial configuration and settings
// Chapter 3. Saving & Loading settings and modifying settings using the toolbox
// Chapter 4. User interaction - Context Menus
// Chapter 5. User interaction - Toolbar Buttons
// Chapter 6. Navigation Data - (6a. Using callback API, 6b. Using Observer/Listener model)
// Chapter 7. NMEA 0183 - (7a. Receiving data using callback API, 7b. Using Observer/Listener model)
//            NMEA 0183 - (7c. Transmitting Data using PushNMEABuffer API)
//			  NMEA 0183 - (7d. Transmitting Data using Observer/Lisyener model)
// Chapter 8. NMEA 2000 - (8a. Receiving NMEA 2000 data)
//			  NMEA 2000 - (8b. Transmitting NMEA 2000 data)
// Chapter 9. Plugin Messaging - (9a. Receiving messages using callback API)
//            Plugin Messaging - (9b. Transmit messages using SendPluginMessage API)
//			  Plugin Messaging - (9c. Receiving messages using Observer/Listener model)

#include "demo_plugin.h"

// Global variables accessed by the plugin and various dialogs
#include "demo_globals.h"

// Implements a wxWizard dialog to configure the plugin's initial settings
#include "demo_wizard.h"

// Implements the toolbox page to demonstrate modifying settings from the Toobox page
#include "demo_toolbox.h"

// Implements a dialog to demonstrate modifying settings from the Plugin Preferences option
#include "demo_settings.h"

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

	// Example of adding an item to the root context menu
	auto demoContextMenu = new wxMenuItem(NULL, k_FirstContextMenu, "Demo", "Demo Plugin Menu", wxITEM_NORMAL, NULL);
	demoContextMenuId = AddCanvasContextMenuItem(demoContextMenu, this);

	// Example of adding an item to a sub context menu
	// Valid Sub Menu Names are "Route", "Waypoint", "Track", "AIS")
	auto dscMenu = new wxMenuItem(NULL, k_SecondContextMenu, "AIS Demo", "Demo Plugin AIS Sub Menu", wxITEM_NORMAL, NULL);
	demoAISContextMenuId = AddCanvasContextMenuItemExt(dscMenu, this, "AIS");

	// Example of adding a Toolbar button
	// Firstly obtain the toolbar button icons
	wxString pluginFolder = GetPluginDataDir(PKG_NAME) + wxFileName::GetPathSeparator() + "data" + wxFileName::GetPathSeparator();

	// This assumes the plugin is using Scaled Vector Graphics (SVG)
	wxString normalIcon = pluginFolder + "demo_icon_normal.svg";
	wxString toggledIcon = pluginFolder + "demo_icon_toggled.svg";
	wxString rolloverIcon = pluginFolder + "demo_icon_rollover.svg";

	// Finally add the toolbar button, note also requires INSTALLS_TOOLBAR_TOOL
	// BUG BUG Note that OpenCPN does not implement the rollover state
	demoToolbarId = InsertPlugInToolSVG("Demo", normalIcon,
		rolloverIcon, toggledIcon, wxITEM_CHECK, "Demo", "Demo Plugin Toolbar Description", NULL, -1, 0, this);

	// A flag used to indicate the toggled/untoggled state of the toolbar button
	isToolbarActive = false;

	// Register handler for OpenCPN Navigation Data observer/listener events
	wxDEFINE_EVENT(EVT_NAV_DATA, ObservedEvt);
	listener_nav = std::move(GetListener(NavDataId(), EVT_NAV_DATA, this));
	Bind(EVT_NAV_DATA, [&](ObservedEvt ev) {
		HandleNavData(ev);
		});


	// Notify OpenCPN what callbacks the plugin registers to receive
	return (WANTS_CONFIG | INSTALLS_TOOLBOX_PAGE | WANTS_PREFERENCES | INSTALLS_TOOLBAR_TOOL
		| WANTS_NMEA_EVENTS | WANTS_NMEA_SENTENCES | WANTS_LATE_INIT | WANTS_PLUGIN_MESSAGING);
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
		HandleVHW(ev);
		});

	// Find an outbound NMEA 0183 connection to use with WriteCommDriver
	nmea0183Driver = FindOutboundConnection("nmea0183");
	if (nmea0183Driver.size() > 0) {
		wxLogMessage("Demo Plugin, Using outbound NMEA 0183 network connection: %s", nmea0183Driver);
	}

	// Register subscriber for PGN 130306 Wind
	wxDEFINE_EVENT(EVT_N2K_130306, ObservedEvt);
	NMEA2000Id id_130306 = NMEA2000Id(130306);
	listener_130306 = std::move(GetListener(id_130306, EVT_N2K_130306, this));
	Bind(EVT_N2K_130306, [&](ObservedEvt ev) {
		HandleN2K_130306(ev);
		});

	
	// Register Subcriber for PGN 128259 Boat Speed
	wxDEFINE_EVENT(EVT_N2K_128259, ObservedEvt);
	NMEA2000Id id_128259 = NMEA2000Id(128259);
	listener_128259 = std::move(GetListener(id_128259, EVT_N2K_128259, this));
	Bind(EVT_N2K_128259, [&](ObservedEvt ev) {
		HandleN2K_128259(ev);
		});

	// Find an outbound NMEA 2000 connection to use with WriteCommDriverN2K
	nmea2000Driver = FindOutboundConnection("nmea2000");
	if (!nmea2000Driver.empty()) {
		wxLogMessage("Demo Plugin, Using outbound NMEA 2000 network connection: %s", nmea2000Driver);
		// For the NMEA 2000 interface, plugins need to register what NMEA 2000 PGN's they transmit.
		// This is required for Actisense NGT-1 Adapters
		// Presumably results in a null operation (NOP) for other NMEA 2000 adapters
		// In this demo plugin We will only transmit PGN 130306 Wind Speed and Direction
		std::vector<int> transmittedPGN = { 130306 }; 
		RegisterTXPGNs(nmea2000Driver, transmittedPGN);
	}

	// Register subscriber for OpenCPN Plugin Messaging
	wxDEFINE_EVENT(EVT_OCPN_MSG, ObservedEvt);
	PluginMsgId msg_id = PluginMsgId("OCPN_RTE_ACTIVATED");
	listener_route = std::move(GetListener(msg_id, EVT_OCPN_MSG, this));
	Bind(EVT_OCPN_MSG, [&](ObservedEvt ev) {
		HandleMsg_RouteActivated(ev);
		});

	// Find an interface for Plugin Messaging
	messagingDriver = FindOutboundConnection("internal");
	if (messagingDriver.size() > 0) {
		wxLogMessage("Demo Plugin, Using outbound internal connection: %s", messagingDriver);
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
	auto installationWizard = std::make_unique<DemoWizard>(GetOCPNCanvasWindow());
	if (installationWizard->RunWizard(installationWizard->m_pages.at(0))) {
		SaveSettings();
	}
}

// This API seems to be deprecated?
void DemoPlugin::SetupToolboxPanel(int page_sel, wxNotebook* pnotebook) {
	wxMessageBox(wxString::Format("SetupToolboxPanel invoked: %d", page_sel), "Demo Plugin");
}

// Invoked when the OpenCPN Toolbox OK, Apply or Cancel buttons are pressed
void DemoPlugin::OnCloseToolboxPanel(int page_sel, int ok_apply_cancel) {
	// BUG BUG Why didn't they use standard enums like wxID_OK ??	
	if ((ok_apply_cancel == 0) || (ok_apply_cancel == 4)) {
		SaveSettings();
	}
}

// Invoked at Startup and displayed when the OpenCPN Toolbox is displayed
// Requires INSTALLS_TOOLBOX_PAGE
void DemoPlugin::OnSetupOptions(void) {
	// Add our toolbox to the "User Interface" tab
	auto toolBoxWindow = AddOptionsPage(OptionsParentPI::PI_OPTIONS_PARENT_UI, _("Demo Settings"));
	auto toolboxSizer = new wxBoxSizer(wxVERTICAL);
	toolBoxWindow->SetSizer(toolboxSizer);
	// Create our toolbox panel and add it to the toolbox via the sizer
	auto demoToolbox =  new DemoToolbox(toolBoxWindow);
	toolboxSizer->Add(demoToolbox, 1, wxALL | wxEXPAND);
}

// Invoked from the plugin's preferences option, enabling the user to modify the plugin's settings.
// Requires WANTS_PREFERENCES
void DemoPlugin::ShowPreferencesDialog(wxWindow* parent) {
	auto demoSettings = std::make_unique<DemoSettings>(parent, wxID_ANY, _("Demo Preferences"));
	if (wxID_OK == demoSettings->ShowModal()) {
		SaveSettings();
	}
}

// Invoked when the plugin's context menu items are selected
void DemoPlugin::OnContextMenuItemCallback(int id) {

	if (id == demoContextMenuId) {
		// A plugin can optionally enable/disable their context menus with the following line
		// SetCanvasContextMenuItemGrey(demoContextMenuId, false);
		wxMessageBox(wxString::Format("Demo Context Menu Selected, Menu Id: %d", id), 
			"Demo Plugin");
	}
}

// Invoked when a plugin's context sub menu items are selected
// Note requires an OpenCPN API level of 1.20 or higher
void DemoPlugin::OnContextMenuItemCallbackExt(int id, std::string obj_ident, std::string obj_type, double lat, double lon) {

	if (id == demoAISContextMenuId) {
		wxMessageBox(wxString::Format("Object Id: %d\nObject Identifier (MMSI): %s\nObject Type: %s\nLatitude: %s\nLongitude: %s",
			id, obj_ident.c_str(), obj_type.c_str(),
			toSDMM_PlugIn(1, lat, true), toSDMM_PlugIn(2, lon, true)), "AIS Target Information", wxOK | wxICON_INFORMATION);
	}
}

// Invoked when the plugin's toolbar button is presssed
void DemoPlugin::OnToolbarToolCallback(int id) {
	if (id == demoToolbarId) {
		// Just display a message box. 
		// Note toggling the state of the toolbar while the message box is displayed
		isToolbarActive = !isToolbarActive;
		SetToolbarItemState(id, isToolbarActive);
		wxMessageBox(wxString::Format("Demo Toolbar invoked, Id: %d", id), "Demo Plugin");
		isToolbarActive = !isToolbarActive;
		SetToolbarItemState(id, isToolbarActive);
	}
}

// The "old" method for receiving Navigation Data from OpenCPN. Requires WANTS_NMEA_EVENTS
void DemoPlugin::SetPositionFixEx(PlugIn_Position_Fix_Ex& pfix) {
	// Persist our current position and heading
	// We will use the heading value in later chapters
	currentLatitude = pfix.Lat;
	currentLongitude = pfix.Lon;
	trueHeading = pfix.Hdt;
}

// The "new" method for receiving Navigation Data from OpenCPN. The handler must be 
// registered and the plugin class inherits from the wxWidgets wxEvtHandler class
void DemoPlugin::HandleNavData(ObservedEvt ev) {
	// Persist our current position and heading
	// We will use the heading value in later chapters
	PluginNavdata navdata = GetEventNavdata(ev);
	currentLatitude = navdata.lat;
	currentLongitude = navdata.lon;
	trueHeading = navdata.hdt;

	// As this is invoked by OpenCPN once per second, also use this to generate true wind
	// sentences, using data received from wind and boat speed NMEA 0183 sentences
	CalculateTrueWind();
	// Generate the NMEA 0183 MWV sentence and transmit it.
	// This is using the "old" API.
	// PushNMEABuffer(FormatTrueWindSentence());
	
	// This is using the "new" Observer/Listener model.
	// Only transmit if we have a valid outbound nmea0183 connection
	if (!nmea0183Driver.empty()) {
		wxString sentence = FormatTrueWindSentence();
		SendNMEA0183(nmea0183Driver, sentence.ToStdString());
	}

	// Transmit NMEA 2000 message using Observer/Listener model
	// Only transmit if we have a valid outbound nmea2000 connection
	if (!nmea2000Driver.empty()) {
		std::vector<uint8_t> payload = FormatTrueWindMessage();
		SendNMEA2000(nmea2000Driver, 255, 4, 130306, payload);
	}

	// Transmit an OpenCPN Plugin message using the "old" method
	wxString jsonMessage = FormatTrueWindJSON();
	// SendPluginMessage("Demo_Plugin", jsonMessage);

	// Transmit an OpenCPN Plugin message using the "new" method
	// Note the space between the message id and message body
	wxString pluginMessage = "Demo_Plugin " + jsonMessage;

	if (!messagingDriver.empty()) {
		SendJSONMessage(messagingDriver, pluginMessage.ToStdString());
	}
}

// The "old" method for receiving NMEA 0183 data. The plugin will receive all sentences
// so not necessarily very efficient. Requires WANTS_NMEA_SENTENCES
void DemoPlugin::SetNMEASentence(wxString& sentence) {
	// The sentence is the complete NMEA 0183 sentence as received by OpenCPN, including
	// sentence delimiter, talker id, mnemonic, checksum delimiter (*) and checksum

	// Using the NMEA 0183 libraries as included with OpenCPN
	NMEA0183 parserNMEA0183;
	parserNMEA0183 << sentence;
	// This checks that the sentence is valid (has a talker id, mnemonic & valid checksum)
	if (parserNMEA0183.PreParse()) {
		// For this demo, we're only interested in data generated by a wind transducer
		// $WIMWV,190.0,R,11.4,K,A*1A
		if (parserNMEA0183.LastSentenceIDReceived == "MWV") {
			ParseWind(&parserNMEA0183);
		}
	}
}

// Extract the wind speed & angle from a NMEA 0183 MWV sentence
void DemoPlugin::ParseWind(NMEA0183* nmeaSentence) {
	if (nmeaSentence->Parse()) {
		// Only interested in apparent wind angle rather than true wind angle
		if (nmeaSentence->Mwv.Reference == "R") {
			apparentWindAngle = nmeaSentence->Mwv.WindAngle;
			// Using the helper API fromUsrWindSpeed_Plugin to convert the speed into knots
			// WSPEED_KTS = 0, WSPEED_MS, WSPEED_MPH, WSPEED_KMH
			if (nmeaSentence->Mwv.WindSpeedUnits == "N") {
				// Knots (Nautical Miles per Hour)
				apparentWindSpeed = fromUsrWindSpeed_Plugin(nmeaSentence->Mwv.WindSpeed, 0);
			}
			else if (nmeaSentence->Mwv.WindSpeedUnits == "M") {
				// Metres per second
				apparentWindSpeed = fromUsrWindSpeed_Plugin(nmeaSentence->Mwv.WindSpeed, 1);
			}
			else if (nmeaSentence->Mwv.WindSpeedUnits == "S") {
				// Statute Miles per hour
				apparentWindSpeed = fromUsrWindSpeed_Plugin(nmeaSentence->Mwv.WindSpeed, 2);
			}
			else if (nmeaSentence->Mwv.WindSpeedUnits == "K") {
				// Kilometres per hour
				apparentWindSpeed = fromUsrWindSpeed_Plugin(nmeaSentence->Mwv.WindSpeed, 2);
			}

			// For the time being, we'll just log the data
			// In later Chapters we'll use this data to draw on the canvas
			wxLogMessage("Demo Plugin, Wind Direction: %0.2f, Wind Speed (knots) %0.2f",
				apparentWindAngle, apparentWindSpeed);
		}
	}
}

// Parse NMEA 0183 Speed through Water sentence obtained from observer/listener model
void DemoPlugin::HandleVHW(ObservedEvt ev) {

	NMEA0183Id id_183_vhw("VHW");
	NMEA0183 parserNMEA0183;
	wxString sentence = GetN0183Payload(id_183_vhw, ev);
	parserNMEA0183 << sentence;

	if (parserNMEA0183.Parse()) {
		// Persist the Speed Through Water value, this will be used
		// in subsequent chapters.
		boatSpeed = parserNMEA0183.Vhw.Knots;
	}
}

// Given heading, boat speed and apparent wind angle and speed, calculate true wind angle and direction
void DemoPlugin::CalculateTrueWind(void) {

	if (apparentWindAngle < 180.0f) {
		trueWindAngle = 90.0f - (180.0f / M_PI * atan((apparentWindSpeed * cos(apparentWindAngle * M_PI / 180.0f) - boatSpeed) / (apparentWindSpeed * sin(apparentWindAngle * M_PI / 180.0f))));
	}
	else if (apparentWindAngle > 180.0f) {
		trueWindAngle = 360.0f - (90.0f - (180.0f / M_PI * atan((apparentWindSpeed * cos((180.0f - (apparentWindAngle - 180.0f)) * M_PI / 180.0f) - boatSpeed) / (apparentWindSpeed * sin((180.0f - (apparentWindAngle - 180.0f)) * M_PI / 180.0f)))));
	}
	else {
		trueWindAngle = 180.0f;
	}
	trueWindSpeed = sqrt(pow((apparentWindSpeed * cos(apparentWindAngle * M_PI / 180.0f)) - boatSpeed, 2) + pow(apparentWindSpeed * sin(apparentWindAngle * M_PI / 180.0f), 2));

	trueWindDirection = fmod(trueWindAngle + trueHeading, 360.0f);
}


// Generate NMEA 0183 MWV Sentence for True Wind Angle, using OpenCPN support library 
wxString DemoPlugin::FormatTrueWindSentence(void) {
	NMEA0183 NMEA0183parser;
	SENTENCE NMEASentence;

	NMEA0183parser.TalkerID = "II";
	NMEA0183parser.Mwv.Empty();
	NMEA0183parser.Mwv.WindAngle = trueWindAngle;
	NMEA0183parser.Mwv.Reference = "T";
	NMEA0183parser.Mwv.WindSpeed = trueWindSpeed;
	NMEA0183parser.Mwv.WindSpeedUnits = "N";
	NMEA0183parser.Mwv.IsDataValid = NTrue;

	NMEA0183parser.Mwv.Write(NMEASentence);

	return NMEASentence.Sentence;
}

// Helper function to find required driver handles
std::string DemoPlugin::FindOutboundConnection(const std::string& connectionType) {
	// Iterate through all of the OpenCPN connections
	for (const auto& driver : GetActiveDrivers()) {
		const auto& attributes = GetAttributes(driver);

		auto protocolIterator = attributes.find("protocol");
		auto directionIterator = attributes.find("ioDirection");

		if (protocolIterator != attributes.end() && directionIterator != attributes.end()) {
			// Found a connection matching the required connection and io direction
			if ((connectionType == protocolIterator->second) && ((directionIterator->second == "OUT")
				|| (directionIterator->second == "IN/OUT"))) {
				return driver;
			}
		}
	}
	return {};
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
	NMEA2000Id id_128259(128259);
	std::vector<uint8_t> payload = GetN2000Payload(id_128259, ev);
	unsigned char sequenceId;
	double boatSpeedWaterReferenced;
	double boatSpeedGroundReferenced;
	tN2kSpeedWaterReferenceType waterReferenceType;

	if (ParseN2kPGN128259(payload, sequenceId, boatSpeedWaterReferenced, boatSpeedGroundReferenced, waterReferenceType)) {
		// Convert from NMEA 2000 SI units which are m/s to OpenCPN's core units
		boatSpeed = fromUsrSpeed_Plugin(boatSpeedWaterReferenced, 3);
	}
}

// Parse NMEA 2000 PGN 130306 message (Wind Speed & Angle)
void DemoPlugin::HandleN2K_130306(ObservedEvt ev) {
	NMEA2000Id id_130306(130306);
	std::vector<uint8_t> payload = GetN2000Payload(id_130306, ev);
	unsigned char sequenceId;
	double windSpeed;
	double windAngle;
	tN2kWindReference windReferenceType;

	if (ParseN2kPGN130306(payload, sequenceId, windSpeed, windAngle, windReferenceType)) {
		// Convert from SI Units, m/s and radians to OpenCPN's core units
		apparentWindSpeed = fromUsrSpeed_Plugin(windSpeed, 3);
		apparentWindAngle = windAngle * 180 / M_PI;
	}
}

// Generate NMEA 2000 PGN 130306 message with True Wind Angle
std::vector<uint8_t> DemoPlugin::FormatTrueWindMessage(void) {
	tN2kMsg msg130306;
	SetN2kWindSpeed(msg130306, 1, trueWindSpeed, trueWindAngle, tN2kWindReference::N2kWind_True_boat);
	std::vector<uint8_t> data(msg130306.Data, msg130306.Data + msg130306.DataLen);
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

// Receive OpenCPN Messages, the "old" way
// Requires WANTS_PLUGIN_MESSAGING
void DemoPlugin::SetPluginMessage(wxString& message_id, wxString& message_body) {
	// We'll just log the received message without parsing the message body.
	if (message_id == "OCPN_WPT_ACTIVATED") {
		wxLogMessage("Demo Plugin, Waypoint activated: %s", message_body);
	}
}

// Generate a plugin-specific JSON Message, encoding true wind speed and angle
// Using the bundled wxJSON library to encode the JSON data
wxString DemoPlugin::FormatTrueWindJSON(void) {
	wxJSONValue root;
	wxJSONWriter writer;
	wxString data;
	root["truewind"]["windangle"] = trueWindAngle;
	root["truewind"]["windspeed"] = trueWindSpeed;
	if (root.Size() > 0) {
		writer.Write(root, data);
		return data;
	}
	return {};
}

// The "new" method for receiving Plugin Messages
// In this example subscribing to Route Activation messages
void DemoPlugin::HandleMsg_RouteActivated(ObservedEvt ev) {
	wxJSONValue root;
	wxJSONReader jsonReader;
	PluginMsgId msg_id = PluginMsgId("OCPN_RTE_ACTIVATED");
	std::string message_body = GetPluginMsgPayload(msg_id, ev);
	int jsonErrorCount = jsonReader.Parse(message_body, &root);
	if (jsonErrorCount > 0) {
		wxLogMessage("Demo Plugin, JSON Error in %s", message_body);
		wxArrayString jsonErrorStrings = jsonReader.GetErrors();
		for (auto &it : jsonErrorStrings) {
			wxLogMessage("Demo Plugin, JSON Error: %s",it);
		}
		return;
	}
	else {
		wxString routeName;
		wxString guid;
		if (root.HasMember("Route_activated")) {
			routeName = root["Route_activated"].AsString();
		}
		if (root.HasMember("GUID")) {
			guid = root["GUID"].AsString();
		}
		// Just display the route name and globally unique Id (GUID). In a later chapter 
		// we'll demonstrate retrieving the route details using its guid
		wxMessageBox(wxString::Format("Destination: %s\nGUID: %s", routeName, guid), "Demo Plugin");
	}
}

// Send a Plugin Message using the "new" observer/listener model.
void DemoPlugin::SendJSONMessage(const std::string& driverHandle, const std::string& message) {
	std::vector<uint8_t> payload(message.begin(), message.end());
	auto sharedPointer = std::make_shared<std::vector<uint8_t> >(std::move(payload));
	CommDriverResult result = WriteCommDriver(driverHandle, sharedPointer);
	if (result != RESULT_COMM_NO_ERROR) {
		wxLogMessage("Demo Plugin, Error sending Plugin Message: %d", result);
	}
}


void DemoPlugin::LoadSettings() {
	wxFileConfig* configSettings = GetOCPNConfigObject();
	if (configSettings) {
		configSettings->SetPath("/PlugIns/DemoPlugin");
		configSettings->Read("A_Boolean_Value", &g_someBooleanValue, false);
		configSettings->Read("An_Integer_Value", &g_someIntegerValue, 0);
		configSettings->Read("A_String_Value", &g_someStringValue, wxEmptyString);
	}
}

void DemoPlugin::SaveSettings() {
	wxFileConfig* configSettings = GetOCPNConfigObject();
	if (configSettings) {
		configSettings->SetPath("/PlugIns/DemoPlugin");
		configSettings->Write("A_Boolean_Value", g_someBooleanValue);
		configSettings->Write("An_Integer_Value", g_someIntegerValue);
		configSettings->Write("A_String_Value", g_someStringValue);
	}
}