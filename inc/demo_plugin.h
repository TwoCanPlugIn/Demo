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
#include <wx/graphics.h>
#include <wx/glcanvas.h>
#include <wx/webrequest.h>

// Defines version numbers, names etc. for this plugin
// This is automagically constructed via config.h.in from CMakeLists.txt
#include "config.h"

// OpenCPN Plugin header
#include "ocpn_plugin.h"

// Bundled OpenCPN JSON libraries
#include "wx/json_defs.h"
#include "wx/jsonreader.h"
#include "wx/jsonval.h"
#include "wx/jsonwriter.h"

// Bundled OpenCPN Device Context layer that supports both OpenGL and non OpenGL
#include "pidc.h"

#include "demo_globals.h"

// Implements a wxWizard dialog to configure the plugin's initial settings
#include "demo_wizard.h"

// Implements the toolbox page to demonstrate modifying settings from the Toobox page
#include "demo_toolbox.h"

// Implements a dialog to demonstrate modifying settings from the Plugin Preferences option
#include "demo_settings.h"

// Dialog to display Active Captain POI details
#include "demo_activecaptain.h"

class DemoPlugin : public opencpn_plugin_120, public wxEvtHandler {
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
	void OnCloseToolboxPanel(int page_sel, int ok_apply_cancel) override;
	void OnSetupOptions(void) override;
	void ShowPreferencesDialog(wxWindow* parent) override;
	void OnContextMenuItemCallback(int id) override;
	void OnToolbarToolCallback(int id) override;
	bool RenderOverlayMultiCanvas(wxDC& dc, PlugIn_ViewPort* vp, int canvasIndex, int priority) override;
	bool RenderGLOverlayMultiCanvas(wxGLContext* pcontext, PlugIn_ViewPort* vp,
		int canvasIndex, int priority) override;
	void SetCurrentViewPort(PlugIn_ViewPort& vp) override;
	void SetCursorLatLon(double lat, double lon) override;
	bool MouseEventHook(wxMouseEvent& event) override;

private:
	// Save and Load configuration Settings
	void LoadSettings();
	void SaveSettings();

	// Context Menu Id's
	int garminContextMenuId;
	
	// Toolbar button Id & state
	int garminToolBarId;
	bool isToolbarActive;

	// Chart Viewport (scale, chart extents etc.)
	PlugIn_ViewPort viewPort;

	// Determine if the cursor is over a point of interest;
	wxString pointOfInterestId;
	bool IsUnderCursor(const double lat, const double lon, wxString* id);

	// Garmin Active Captain Points Of Interest(s) related functions
	bool RequestGarminPointOfInterest(void);
	bool RequestGarminPointOfInterest(const wxString& id);
	void SavePointOfInterest(const wxString& response);
	bool IsDuplicate(ActiveCaptainPOI& poi);
	void DisplayPointOfInterest(const wxString& reponse);
	std::vector<ActiveCaptainPOI> pointsOfInterest;
	Category StringToCategory(const wxString& s);
	wxString CategoryToString(const Category& c);

};

#endif 

