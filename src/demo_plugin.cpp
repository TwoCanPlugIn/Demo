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
	return (WANTS_CONFIG | INSTALLS_TOOLBOX_PAGE | WANTS_PREFERENCES);
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
void DemoPlugin::OnSetupOptions(void) {
	// Add our toolbox to the "User Interface" tab
	auto toolBoxWindow = AddOptionsPage(OptionsParentPI::PI_OPTIONS_PARENT_UI, _("Demo Settings"));
	auto toolboxSizer = new wxBoxSizer(wxVERTICAL);
	toolBoxWindow->SetSizer(toolboxSizer);
	// Create our toolbox panel and add it to the toolbox via the sizer
	auto demoToolbox =  new DemoToolbox(toolBoxWindow);
	toolboxSizer->Add(demoToolbox, 1, wxALL | wxEXPAND);
}

void DemoPlugin::ShowPreferencesDialog(wxWindow* parent) {
	auto demoSettings = std::make_unique<DemoSettings>(parent, wxID_ANY, _("Demo Preferences"));
	if (wxID_OK == demoSettings->ShowModal()) {
		SaveSettings();
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