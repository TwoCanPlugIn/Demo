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
// along with the Racing plugin for OpenCPN. If not, see <https://www.gnu.org/licenses/>.
//

//
// Project: Demo Plugin
// Description: Demonstrate the use of the OpenCPN plugin API's
// Owner: twocanplugin@hotmail.com
// Date: 10/01/2025
// Version History: 
// 1.0 Initial Release

#include "demo_plugin.h"

// wxWidgets includes
#include <wx/window.h>
#include <wx/filename.h>

// The class factories, used to create and destroy instances of the PlugIn
extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr) {
	return new DemoPlugin(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) {
	delete p;
}

// Constructor
// This release is a basic plugin that does not require any "newer" plugin API's beyond API v 1.7
DemoPlugin::DemoPlugin(void* ppimgr) : opencpn_plugin_117(ppimgr) {
	
	// Initialize the plugin bitmap, converting from SVG to PNG. Refer to GetPluginBitmap
	// The svg file will be "installed: into the plugin's data folder
	wxString pluginFolder = GetPluginDataDir(PKG_NAME) + wxFileName::GetPathSeparator() + "data" + wxFileName::GetPathSeparator();
	m_PluginBitmap = GetBitmapFromSVGFile(pluginFolder + "demo_plugin.svg", 32, 32);
}

// Destructor
DemoPlugin::~DemoPlugin(void) {
}

// Perform plugin initialization here. At this point most of OpenCPN has been initialised and most of the plugin API's can be invoked
int DemoPlugin::Init(void) {

	// Maintain a reference to the OpenCPN window to use as the parent Window for plugin dialogs
	m_ParentWindow = GetOCPNCanvasWindow();

	// Load localized strings. Strings to be localized are prefixed with the _() macro
	// BUG BUG Naming...
	// AddLocaleCatalog("opencpn-demo_pi");

	// Dump some of the OpenCPN's special folders
	wxLogMessage("Demo Plugin, OpenCPN Program Path (opencpn.exe): %s", GetOCPN_ExePath());
	wxLogMessage("Demo Plugin, OpenCPN Plugin Path (built-in plugins): %s", *GetpPlugInLocation());
	wxLogMessage("Demo Plugin, OpenCPN Private Data Path (logs, config): %s", *GetpPrivateApplicationDataLocation());
	wxLogMessage("Demo Plugin, OpenCPN Shared Data Path: %s", *GetpSharedDataLocation());
	wxLogMessage("Demo Plugin, Any Plugin's Writeable Documents Path: %s", GetWritableDocumentsDir());
	wxLogMessage("Demo Plugin, This Plugin's Data Path: %s", GetPluginDataDir(PKG_NAME));
	wxLogMessage("Demo Plugin, This Plugin's Library Path: %s", GetPlugInPath(this));
	
	// Notify OpenCPN what events we want to receive callbacks for
	return 0;
}

// OpenCPN is either closing down, or we have been disabled from the Preferences Dialog
bool DemoPlugin::DeInit(void) {

	// OpenCPN doesn't actually care about the return value
	return true; 
}

// OpenCPN Plugin "housekeeping" methods. All plugins MUST implement these

// Indicate what version of the OpenCPN Plugin API we support
// Indicate what version of the OpenCPN Plugin API we support
int DemoPlugin::GetAPIVersionMajor() {
	return atoi(API_VERSION);
}

// shipdriver template is rather fugly compared to testplugin template....
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
// Shipdriver template fugliness....
wxString DemoPlugin::GetCommonName() {
	return PLUGIN_API_NAME;
}

wxString DemoPlugin::GetShortDescription() {
	return PKG_SUMMARY;
}

wxString DemoPlugin::GetLongDescription() {
	return PKG_DESCRIPTION; //PLUGIN_LONG_DESCRIPTION;
}

// Most plugins use a 32x32 pixel PNG file converted to xpm by pgn2wx.pl perl script
// However easier to use a SVG file as it means I can maintain one image format rather than several.
wxBitmap* DemoPlugin::GetPlugInBitmap() {
	return &pluginBitmap;
}

