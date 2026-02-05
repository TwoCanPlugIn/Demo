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

#ifndef DEMO_SETTINGS_H
#define DEMO_SETTINGS_H

// wxWidgets Precompiled Headers
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif

// The dialog base class from which we are derived
// Note wxFormBuilder was used to generate the UI
#include "demo_ui_base.h"

#include "demo_globals.h"

class DemoSettings : public DemoSettingsBase {

public:
	DemoSettings(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Demo Plugin Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	~DemoSettings();

protected:
	// Overridden methods from the base class
	void OnInit(wxInitDialogEvent& event) override;
	void OnOK(wxCommandEvent& event) override;
	void OnCancel(wxCommandEvent& event) override;
	void OnApply(wxCommandEvent& event) override;


private:
	bool originalBooleanValue;
	int originalIntegerValue;
	wxString originalStringValue;
};

#endif
