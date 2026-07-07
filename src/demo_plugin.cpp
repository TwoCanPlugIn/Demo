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
// Project: GPX Export
// Description: Derived from Demo plugin, demonstrate export of waypoints formatted using GPX
// Owner: twocanplugin@hotmail.com
// Date: 31/03/2026
// Version History: 
// 1.0 Initial Release
// 03/05/2026 - 1.1 Export waypoints using GeoJSON format
// 14/05/2026 - 1.2 Export Waypoints to SignalK
// 30/06/2026 - 1.3 Export Routes using RTZ and IHO S-421

#include "demo_plugin.h"

// Global variables accessed by the plugin and various dialogs
#include "demo_globals.h"

// Implements a wxWizard dialog to configure the plugin's initial settings
#include "demo_wizard.h"

// Implements the toolbox page to demonstrate modifying settings from the Toobox page
#include "demo_toolbox.h"

// Implements a dialog to demonstrate modifying settings from the Plugin Preferences option
#include "demo_settings.h"

// Implements a class that exports routes in RTZ format
#include "demo_rtz.h"

// Implements a class that exports routes in S-421 format
#include "demo_s421.h"

// The class factories, used to create and destroy instances of the PlugIn
extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr) {
	return new DemoPlugin(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) {
	delete p;
}

// Constructor
// This release is a basic plugin that does not require any "newer" plugin API's beyond API v 1.17
DemoPlugin::DemoPlugin(void* ppimgr) : opencpn_plugin_120(ppimgr) , wxEvtHandler() {
	
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

	// Dump the location of the user's documents folder
	wxLogMessage("Demo Plugin, User's Documents: %s", wxStandardPaths::Get().GetDocumentsDir());
	
	// Load the previously saved settings
	LoadSettings();

	// Example of adding an item to a sub context menu
	// Valid Sub Menu Names are "Route", "Waypoint", "Track", "AIS")
	// Export waypoint as either GPX or GeoJSON
	auto exportMenu = new wxMenuItem(NULL, k_SecondContextMenu, "Export Waypoint", "Export Waypoint as GPX or GeoJSON", wxITEM_NORMAL, NULL);
	exportWaypointMenuId = AddCanvasContextMenuItemExt(exportMenu, this, "Waypoint");

	// Export waypoint to SignalK
	auto signalKMenu = new wxMenuItem(NULL, k_SecondContextMenu, "Export to SignalK", "Export Waypoint to SignalK", wxITEM_NORMAL, NULL);
	exportSignalKMenuId = AddCanvasContextMenuItemExt(signalKMenu, this, "Waypoint");

	// Export route to RTZ
	auto rtzMenu = new wxMenuItem(NULL, k_ThirdContextMenu, "Export RTZ", "Export Route using RTZ", wxITEM_NORMAL, NULL);
	exportRTZMenuId = AddCanvasContextMenuItemExt(rtzMenu, this, "Route");	

	// Export route to S-421
	auto s421Menu = new wxMenuItem(NULL, k_FourthContextMenu, "Export S-421", "Export Route using S-421", wxITEM_NORMAL, NULL);
	exportS421MenuId = AddCanvasContextMenuItemExt(s421Menu, this, "Route");

	// Example of adding a Toolbar button
	// Firstly obtain the toolbar button icons
	wxString pluginFolder = GetPluginDataDir(PKG_NAME) + wxFileName::GetPathSeparator() + "data" + wxFileName::GetPathSeparator();

	// This assumes the plugin is using Scaled Vector Graphics (SVG)
	wxString normalIcon = pluginFolder + "demo_icon_normal.svg";
	wxString toggledIcon = pluginFolder + "demo_icon_toggled.svg";
	wxString rolloverIcon = pluginFolder + "demo_icon_rollover.svg";

	// Finally add the toolbar button, note also requires INSTALLS_TOOLBAR_TOOL
	// BUG BUG Note that OpenCPN does not implement the rollover state
	exportWaypointsToolbarId = InsertPlugInToolSVG("Demo", normalIcon,
		rolloverIcon, toggledIcon, wxITEM_CHECK, "Export Waypoints", "Export all waypoints", NULL, -1, 0, this);

	// A flag used to indicate the toggled/untoggled state of the toolbar button
	isToolbarActive = false;

	
	// Notify OpenCPN what callbacks the plugin registers to receive
	return (WANTS_CONFIG | INSTALLS_TOOLBOX_PAGE | WANTS_PREFERENCES | INSTALLS_TOOLBAR_TOOL);
}

// OpenCPN is either closing down, or the plugin has been disabled from the Preferences Dialog
bool DemoPlugin::DeInit(void) {

	// Note, OpenCPN doesn't actually care about the return value
	return true; 
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

// Invoked from he plugin's preferences option, enabling the user to modify the plugin's settings.
// Requires WANTS_PREFERENCES
void DemoPlugin::ShowPreferencesDialog(wxWindow* parent) {
	auto demoSettings = std::make_unique<DemoSettings>(parent, wxID_ANY, _("Demo Preferences"));
	if (wxID_OK == demoSettings->ShowModal()) {
		SaveSettings();
	}
}

// Export the selected waypoint using either GPX or GeoJSON format
void DemoPlugin::OnContextMenuItemCallbackExt(int id, std::string obj_ident, std::string obj_type, double lat, double lon) {
	if (id == exportWaypointMenuId) {
		wxArrayString guids;
		guids.Add(obj_ident);
		ExportWaypoints(guids);
	}

	//  Obtain the SignalK Server address & port
	if (id == exportSignalKMenuId) {
		wxString signalKServerAddress;
		wxString signalKServerPort;
		if (GetSignalKServer(&signalKServerAddress, &signalKServerPort)) {
			wxString text = ExportToSignalK(obj_ident);
			wxString uri = wxString::Format("http://%s:%s/signalk/v2/api/resources/waypoints",
				signalKServerAddress, signalKServerPort);
				PostToSignalK(uri, text);
		}
	}

	if (id == exportRTZMenuId) {
		ExportToRTZ(obj_ident);
	}

	if (id == exportS421MenuId) {
		ExportToS421(obj_ident);
	}
}

// Export all waypoints using either GPX or GeoJSON format
void DemoPlugin::OnToolbarToolCallback(int id) {
	if (id == exportWaypointsToolbarId) {
		isToolbarActive = !isToolbarActive;
		SetToolbarItemState(id, isToolbarActive);
		ExportWaypoints(GetWaypointGUIDArray());
		isToolbarActive = !isToolbarActive;
		SetToolbarItemState(id, isToolbarActive);
	}
}

// Display a file save dialog and depending on the file extension export as GPX or GeoJSON
void DemoPlugin::ExportWaypoints(const wxArrayString& guids) {

	wxFileDialog fileSaveDialog(GetOCPNCanvasWindow(), _("Export Waypoints"),
		wxStandardPaths::Get().GetDocumentsDir(), "", "GPX (*.gpx)|*.gpx|GeoJson (*.txt)|*.txt",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (fileSaveDialog.ShowModal() == wxID_OK) {
		wxFileName filePath(fileSaveDialog.GetPath());

		// Output as GPX
		if (filePath.GetExt().compare("gpx") == 0) {
			ExportAsGPX(fileSaveDialog.GetPath(), guids);
		}

		// Output as GeoJson
		else if (filePath.GetExt().compare("txt") == 0) {
			ExportAsGeoJson(fileSaveDialog.GetPath(), guids);
		}

		else {
			wxMessageBox("Select .gpx or .txt for GPX or GeoJSON files", "Demo Plugin");
		}
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

// Export waypoint(s) formatted using GPX
void DemoPlugin::ExportAsGPX(const wxString& fileName, const wxArrayString& guids) {
	pugi::xml_document doc;
	// XML Declaration
	pugi::xml_node decl = doc.append_child(pugi::node_declaration);
	decl.append_attribute("version") = "1.0";
	decl.append_attribute("encoding") = "UTF-8";
	// GPX metadata
	pugi::xml_node gpx = doc.append_child("gpx");
	gpx.append_attribute("version") = "1.1";
	gpx.append_attribute("creator") = "Demo Plugin";
	gpx.append_attribute("xmlns") = "http://www.topografix.com/GPX/1/1";

	// waypoints
	pugi::xml_node node;
	pugi::xml_node value;
	PlugIn_Waypoint wpt;
	for (auto it : guids) {
		GetSingleWaypoint(it, &wpt);
		node = gpx.append_child("wpt");
		node.append_attribute("lat") = wpt.m_lat;
		node.append_attribute("lon") = wpt.m_lon;
		value = node.append_child("name");
		value.append_child(pugi::node_pcdata).set_value(wpt.m_MarkName);
		value = node.append_child("desc");
		value.append_child(pugi::node_pcdata).set_value(wpt.m_MarkDescription);
	}

	if (doc.save_file(fileName.ToAscii().data())) {
		wxMessageBox("Saved GPX file " + fileName);
	}
	else {
		wxMessageBox("Error saving GPX file " + fileName);
	}
}

// Export waypoint(s) formatted GeoJSON
void DemoPlugin::ExportAsGeoJson(const wxString& fileName, const wxArrayString& guids) {
	wxJSONValue root;
	
	root["type"] = wxString("FeatureCollection");

	// Array of features (we're just exporting waypoints that correspond to a point)
	// I guess routes would be exported as a line.
	wxJSONValue features(wxJSONTYPE_ARRAY);

	PlugIn_Waypoint wpt;
	for (auto it : guids) {
		GetSingleWaypoint(it, &wpt);

		wxJSONValue feature;
		feature["type"] = wxString("Feature");

		// Geometries could be a point, line etc.
		wxJSONValue geometry;
		geometry["type"] = wxString("Point");

		// WTF, longitude first!
		wxJSONValue coordinates(wxJSONTYPE_ARRAY);
		coordinates.Append(wpt.m_lon);
		coordinates.Append(wpt.m_lat);

		geometry["coordinates"] = coordinates;

		// Assign geometry to the feature
		feature["geometry"] = geometry;

		// Assign name & description to properties
		wxJSONValue properties;
		properties["name"] = wpt.m_MarkName;
		properties["description"] = wpt.m_MarkDescription;
		// Assign properties to the feature
		feature["properties"] = properties;

		// Add the feature to the features array
		features.Append(feature);
	}

	// Finally assign features to the root object
	root["features"] = features;

	// Generate the string output
	wxJSONWriter jsonWriter(wxJSONWRITER_STYLED);
	wxString jsonOutput;
	jsonWriter.Write(root, jsonOutput);

	wxFile outputFile;
	if (outputFile.Open(fileName, wxFile::write)) {
		if (outputFile.Write(jsonOutput)) {
			outputFile.Close();
			wxMessageBox("Saved GeoJSON file " + fileName);
		}
	}
	else {
		wxMessageBox("Error saving GeoJSON file " + fileName);
	}
}

// Export Waypoint to SignalK, constructs the GeoJson object
// BUG BUG should refactor with the GeoJson code
wxString DemoPlugin::ExportToSignalK(std::string& guid) {
	
	PlugIn_Waypoint wpt;
	GetSingleWaypoint(guid, &wpt);

	wxJSONValue root;
	
	// Top-level fields
	root["name"] = wpt.m_MarkName;
	root["description"] = wpt.m_MarkDescription;
	root["type"] = wxEmptyString;
	
	// Feature object
	wxJSONValue feature;
	feature["type"] = wxString("Feature");
	// Empty properties object
	wxJSONValue properties(wxJSONTYPE_OBJECT);
	feature["properties"] = properties;
	// Empty feature ID
	feature["id"] = wxEmptyString; // Possibly wpt.m_GUID

	// Geometry object
	wxJSONValue geometry;
	geometry["type"] = wxString("Point");
	wxJSONValue coords(wxJSONTYPE_ARRAY);
	coords.Append(wpt.m_lon); 
	coords.Append(wpt.m_lat);
	geometry["coordinates"] = coords;

	// Add geometry to feature
	feature["geometry"] = geometry;

	// Add feature to root
	root["feature"] = feature;

	// Convert JSON object to string
	wxJSONWriter writer;
	wxString jsonString;
	writer.Write(root, jsonString);

	return jsonString;
}

// Send the waypoint as GeoJson to the SignalK Resource Provider
bool DemoPlugin::PostToSignalK(wxString& url, wxString& jsonText) {

	bool result = false;
	// Create the request object
	wxWebRequest request = wxWebSession::GetDefault().CreateRequest(this, url);
	
	// Specify the Post method
	request.SetMethod("Post");
	// Authorization
	// BUG BUG This should be stored securely
	request.SetHeader("Authorization", "Bearer gobbledygook");
	
	// Content Type
	request.SetHeader("Content-Type", "application/json; charset=UTF-8");

	// Finally the data itself.
	request.SetData(jsonText, "application/json");

	if (!request.IsOk()) {
		wxLogError("Demo Plugin, Unexepected Web Request Error");
		return false;
	}

	// The web request event handler (just a lambda function)
	Bind(wxEVT_WEBREQUEST_STATE, [&result](wxWebRequestEvent& evt) {
		switch (evt.GetState())	{
			case wxWebRequest::State_Completed: 
				wxMessageBox("Sent Waypoint to SignalK", "Demo Plugin");
				result = true;
			break;

			case wxWebRequest::State_Failed:
				wxMessageBox(wxString::Format("Failed to Send Waypoint to SignalK %s", 
					evt.GetErrorDescription()), "Demo Plugin");
				result = false;
			break;
			// Possibly handle	wxWebRequest::State_Unauthorized etc.
		}
	});

	// Start the request
	request.Start();

	return result;
}

bool DemoPlugin::ExportToRTZ(std::string& guid) {
	// Retrieve the route details
	auto routeDetails = GetRouteExV2_Plugin(wxString(guid));
	auto waypointList = routeDetails->pWaypointList;

	// Format the XML document 
	// Note using g_someStringValue to store the vessel's name.
	auto rtzRoute = std::make_unique <DemoRTZ>();
	rtzRoute->CreateRoute(routeDetails->m_NameString.ToStdString(), 
		g_someStringValue.ToStdString());

	// OpenCPN needs to define further attributes to populate these fields
	Schedule rtzSchedule;

	int waypointId = 0;
	for (Plugin_WaypointExV2List::iterator it = waypointList->begin();
		it != waypointList->end(); ++it) {

		auto waypoint = *it;

		// RTZ Leg Information
		LegAttributes legAttribute;
		legAttribute.geometryType = "Loxodrome"; 
		// For long routes, while OpenCPN will use a great circle, each of the 
		// generated waypoints are rhumb lines
		legAttribute.speedMax = legAttribute.speedMin = waypoint->m_PlannedSpeed;
		legAttribute.legInfo = wxString::Format("WPT%03d", waypointId).ToStdString(); // waypoint->m_MarkDescription.ToStdString();
		// BUG BUG Could copy the hyperlinks to legNote1 or legNote2
		
		// Add the waypoint information (including leg information)
		rtzRoute->AddWaypoint(waypointId, 1, waypoint->m_lat, waypoint->m_lon,
			waypoint->m_MarkName.ToStdString(), waypoint->m_WaypointArrivalRadius, legAttribute);

		// RTZ Schedule Information
		ScheduleElement scheduleAttribute;
		scheduleAttribute.etd = waypoint->m_ETD.FormatISOCombined().ToStdString();
		scheduleAttribute.speed = waypoint->m_PlannedSpeed;
		scheduleAttribute.waypointId = waypointId;

		// OpenCPN has no mechanism to manually enter in schedule information
		// so we only populate the calculated RTZ schedule field
		rtzSchedule.calculated.push_back(scheduleAttribute);

		waypointId++;
	}

	rtzSchedule.name= wxString::Format("From %s to %s", routeDetails->m_StartString.ToStdString(), 
		routeDetails->m_EndString).ToStdString();
	rtzSchedule.id = 1;
	rtzRoute->AddSchedule(rtzSchedule);

	// Prompt the user for a file name
	wxFileDialog fileSaveDialog(GetOCPNCanvasWindow(), _("Export Route as RTZ"),
		wxStandardPaths::Get().GetDocumentsDir(), "", "RTZ (*.rtz)|*.rtz",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	// And save the RTZ file
	if (fileSaveDialog.ShowModal() == wxID_OK) {
		if (rtzRoute->Save(fileSaveDialog.GetPath().ToStdString())) {
			wxMessageBox("Saved RTZ file", "Demo Plugin");
			return true;
		}
		else {
			wxMessageBox("Failed to save RTZ file", "Demo Plugin");
		}
	}
	return false;
}

bool DemoPlugin::ExportToS421(std::string& guid) {

	// Retrieve the route details
	auto routeDetails = GetRouteExV2_Plugin(wxString(guid));
	auto waypointList = routeDetails->pWaypointList;

	// Format the XML document
	auto s421Route = std::make_unique <DemoS421>();

	s421Route->CreateDataset("DemoPlugin");

	s421Route->AddRouteInfo(routeDetails->m_NameString.ToStdString(), "OpenCPN", 
		GetActiveRouteGUID() == routeDetails->m_GUID ? "Active" : "Planned");

	S421Schedule schedule;
	schedule.id = 1;
	schedule.name = wxString::Format("From %s to %s", routeDetails->m_StartString,
		routeDetails->m_EndString).ToStdString();

	int waypointId = 0;
	for (Plugin_WaypointExV2List::iterator it = waypointList->begin();
		it != waypointList->end(); ++it) {
		auto waypoint = *it;

		S421Waypoint s421Waypoint;
		s421Waypoint.id = waypointId;
		s421Waypoint.lat = waypoint->m_lat;
		s421Waypoint.lon = waypoint->m_lon;
		s421Waypoint.name = waypoint->m_MarkName.ToStdString();
		s421Route->AddWaypoint(s421Waypoint);

		S421ScheduleElement se;
		se.waypointId = waypointId;
		se.etd = waypoint->m_ETD.FormatISOCombined().ToStdString();
		se.speed = waypoint->m_PlannedSpeed;
		// OpenCPN doesn't have a waypoint ETA field.

		schedule.calculated.push_back(se);

		waypointId++;
	}

	s421Route->AddSchedule(schedule);

	// Prompt the user for a file name
	wxFileDialog fileSaveDialog(GetOCPNCanvasWindow(), _("Export Route as S-421"),
		wxStandardPaths::Get().GetDocumentsDir(), "", "S421 (*.s421)|*.s421",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	// And save the S421 file
	if (fileSaveDialog.ShowModal() == wxID_OK) {
		// This also generates the metdata etc.
		if (s421Route->SaveHDF5(fileSaveDialog.GetPath().ToStdString())) {
			wxMessageBox("Saved S-421 file", "Demo Plugin");
			return true;
		}
		else {
			wxMessageBox("Failed to save S-421 file", "Demo Plugin");
		}
	}
	return false;
}

// Iterate thedata connections to get the IP address of a SignalK Server connection
bool DemoPlugin::GetSignalKServer(wxString* ipAddress, wxString* ipPort) {
	// Handle to OpenCPN connection object
	wxFileConfig* configSettings = GetOCPNConfigObject();
	wxString dataConnections;

	if (configSettings) {
		configSettings->SetPath("/Settings/NMEADataSource");
		configSettings->Read("DataConnections", &dataConnections, wxEmptyString);

		if (!dataConnections.IsEmpty()) {
			// Iterate each connection delimited by |
			wxArrayString connections = wxStringTokenize(dataConnections, "|");
			for (const auto& connection : connections) {
				// Iterate each elements delimited by ;
				wxArrayString items = wxStringTokenize(connection, ";");
				// Connection Types: 0 = UDP, 1 = TCP, 2 = GPSD, 3 = SignalK 
				if (items[1] == "3") {
					*ipAddress = items[2];
					*ipPort = items[3];
					return true;
				}
			}
			wxLogMessage("Demo Plugin, Error no SignalK Server connection");
		}
		else {
			wxLogMessage("Demo Plugin, Error no Data Connections");
		}
	}
	else {
		wxMessageBox("Demo Plugin, Error getting OpenCPN settings");
	}
	return false;
}
