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
// This release is a basic plugin that does not require any "newer" plugin API's beyond API v 1.17
DemoPlugin::DemoPlugin(void* ppimgr) : opencpn_plugin_120(ppimgr) {
	
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
	auto gpxMenu = new wxMenuItem(NULL, k_SecondContextMenu, "Export GPX", "Export Waypoint as GPX", wxITEM_NORMAL, NULL);
	exportWaypointMenuId = AddCanvasContextMenuItemExt(gpxMenu, this, "Waypoint");

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

// Export the selected waypoint
void DemoPlugin::OnContextMenuItemCallbackExt(int id, std::string obj_ident, std::string obj_type, double lat, double lon) {
	if (id == exportWaypointMenuId) {

		wxFileDialog fileSaveDialog(GetOCPNCanvasWindow(), _("Export GPX"),
			wxStandardPaths::Get().GetDocumentsDir(), "", "GPX files (*.gpx)|*.gpx",
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

		if (fileSaveDialog.ShowModal() == wxID_OK) {
			wxArrayString guids;
			guids.Add(obj_ident);
			FormatAsGPX(fileSaveDialog.GetPath(), guids);
		}
	}
}

// Export all waypoints using GPX format
void DemoPlugin::OnToolbarToolCallback(int id) {
	if (id == exportWaypointsToolbarId) {
		isToolbarActive = !isToolbarActive;
		SetToolbarItemState(id, isToolbarActive);
		
		wxFileDialog fileSaveDialog(GetOCPNCanvasWindow(), _("Export GPX"),
			wxStandardPaths::Get().GetDocumentsDir(), "", "GPX files (*.gpx)|*.gpx",
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

		if (fileSaveDialog.ShowModal() == wxID_OK) {
			FormatAsGPX(fileSaveDialog.GetPath(), GetWaypointGUIDArray());
		}
		
		isToolbarActive = !isToolbarActive;
		SetToolbarItemState(id, isToolbarActive);
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
void DemoPlugin::FormatAsGPX(wxString fileName, wxArrayString guids) {
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