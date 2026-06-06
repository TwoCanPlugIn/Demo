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

#ifndef DEMO_GATEWAY_UI_H
#define DEMO_GATEWAY_UI_H

// wxWidgets Precompiled Headers
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif

#include <map>
#include <functional>

// The dialog base class from which we are derived
// Note wxFormBuilder was used to generate the UI
#include "demo_ui_base.h"

#include "demo_globals.h"

#include "ocpn_plugin.h"

class DemoGatewayUI : public DemoGatewayUIBase {

public:
	DemoGatewayUI(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Demo Plugin Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	~DemoGatewayUI();

protected:
	// Overridden methods from the base class
	void OnInit(wxInitDialogEvent& event) override;
	void OnOK(wxCommandEvent& event) override;
	void OnCancel(wxCommandEvent& event) override;
	void OnInterfaceAChanged(wxCommandEvent& event) override;
	void OnInterfaceBChanged(wxCommandEvent& event) override;
	void OnCellChanged(wxGridEvent& event) override;
	void OnCellChanging(wxGridEvent& event) override;
	void OnLabelClicked(wxGridEvent& event) override;

private:
	// Toggle state of the two columns
	bool toggle[2] = { false, false };

	// Associatate grid rows with their respective variables via lambda functions 
	std::map<int, std::function<void(const ConversionType&)>> gridRowSetter;
	std::map<int, std::function<ConversionType()>> gridRowGetter;

	// Helper function to find a required connection (refactored from the "original" demo plugin)
	std::unordered_map<DriverHandle, std::string> FindOutboundConnection(const std::string& connectionType);

	// Unused, Given a value, return the matching key from the unordered mao
	int GetKeyFromValue(const std::unordered_map<int, std::string>& map,
		const std::string& value);
};

#endif
