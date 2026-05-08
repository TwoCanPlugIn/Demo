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

#ifndef DEMO_DASHBOARD_H
#define DEMO_DASHBOARD_H

// wxWidgets Precompiled Headers
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif
#include <wx/clipbrd.h>
#include <wx/aui/framemanager.h>

// The dialog base class from which we are derived
// Note wxFormBuilder was used to generate the UI
#include "demo_ui_base.h"

// We use some of the OpenCPN functions to display position and speed in the user's units
#include "ocpn_plugin.h"

// A quick and dirty compass
class Compass : public wxPanel {
public:
    Compass(wxWindow* parent);
    ~Compass() = default;
    void SetHeading(double deg);
	void SetNightMode(bool mode);

private:
	double heading = 0.0;
    wxBitmap backgroundBitmap;
	bool nightMode = false;
    void OnPaint(wxPaintEvent&);
    void OnSize(wxSizeEvent& evt); 
    void DrawBackground(); 
	void DrawNeedle(wxDC& dc);
};

// Quick and dirty Dashboard for the wxAUI demo
class DemoDashboard : public DemoDashboardBase {
	
public:
	DemoDashboard(wxWindow* parent);
	~DemoDashboard();

	void SetPosition(const double& latitude, const double& longitude);
	void SetWindAngle(const double& angle);
	void SetWindSpeed(const double& speed);
	void SetMagneticHeading(const double& heading);
	void SetNightMode(bool mode);
	
protected:
	// Overridden methods from the base class
	void OnButtonClose(wxCommandEvent& event) override;
	void OnButtonCopy(wxCommandEvent& event) override;
	
private:
	// A simple compass rose
	std::unique_ptr<Compass> compass;
};

#endif
