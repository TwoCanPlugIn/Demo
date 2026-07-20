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

#ifndef DEMO_POI_H
#define DEMO_POI_H

// wxWidgets Precompiled Headers
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif

// The dialog base class from which we are derived
// Note wxFormBuilder was used to generate the UI
#include "demo_ui_base.h"

#include "demo_globals.h"

#include "ocpn_plugin.h"

class DemoActiveCaptain : public DemoActiveCaptainBase {

public:
	DemoActiveCaptain(const ActiveCaptainPOI& poiDetails, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Active Captain Point of Interest"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	~DemoActiveCaptain();

protected:
	// Overridden methods from the base class
	void OnInit(wxInitDialogEvent& event) override;
	void OnOK(wxCommandEvent& event) override;

private:
	ActiveCaptainPOI poi;
	
};

#endif
