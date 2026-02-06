// Copyright(C) 2026 by Steven Adler
//
// This file is part of Demo plugin for OpenCPN.
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


#ifndef DEMO_PLUGIN_H
#define DEMO_PLUGIN_H

// Pre compiled headers 
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif

// wxWidgets
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/fileconf.h>

// Defines version numbers, names etc. for this plugin
// This is automagically constructed via config.h.in from CMakeLists.txt
#include "config.h"

// OpenCPN Plugin header
#include "ocpn_plugin.h"

class DemoPlugin : public opencpn_plugin_120 {
public:
	// Constructor
	DemoPlugin(void* ppimgr);

	// Destructor
	~DemoPlugin(void);

	// Overridden OpenCPN plugin methods
	int Init(void) override;
	bool DeInit(void) override;
	int GetAPIVersionMajor() override;
	int GetAPIVersionMinor() override;
	int GetPlugInVersionMajor() override;
	int GetPlugInVersionMinor() override;
	int GetPlugInVersionPatch() override;
	wxString GetCommonName() override;
	wxString GetShortDescription() override;
	wxString GetLongDescription() override;
	wxBitmap* GetPlugInBitmap() override;
	void SetDefaults(void) override;
	void SetupToolboxPanel(int page_sel, wxNotebook* pnotebook) override;
	void OnCloseToolboxPanel(int page_sel, int ok_apply_cancel) override;
	void OnSetupOptions(void) override;
	void ShowPreferencesDialog(wxWindow* parent) override;
	void OnContextMenuItemCallback(int id) override;
	void OnContextMenuItemCallbackExt(int id, std::string obj_ident, std::string obj_type, double lat, double lon) override;
	void OnToolbarToolCallback(int id) override;

private:
	void LoadSettings();
	void SaveSettings();

	// Context Menu Id's
	int demoContextMenuId;
	int demoAISContextMenuId;
	
	// Toolbar button Id & state
	int demoToolbarId;
	bool isToolbarActive;
};

#endif 

