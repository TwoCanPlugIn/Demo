// Copyright(C) 2018-2025 by Steven Adler
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

#include "demo_plugin.h"

// Global variables accessed by the plugin and various dialogs
#include "demo_globals.h"

// Implements a wxWizard dialog to configure the plugin's initial settings
#include "demo_wizard.h"

// The class factories, used to create and destroy instances of the PlugIn
extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr) {
	return new DemoPlugin(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) {
	delete p;
}

// Constructor
// This release is a basic plugin that does not require any "newer" plugin API's beyond API v 1.17
DemoPlugin::DemoPlugin(void* ppimgr) : opencpn_plugin_117(ppimgr) {
	
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
	
	// Notify OpenCPN what callbacks the plugin registers to receive
	return 0;
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

