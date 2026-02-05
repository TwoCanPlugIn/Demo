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


// Project: Demo Plugin
// Description: A wxWidgets wxDialog to modify the plugin's settings
// Owner: twocanplugin@hotmail.com
// Date: 6/1/2026
// Version History: 
// 1.0 Initial Release
//

#include "demo_settings.h"

// Constructor and destructor implementation
DemoSettings::DemoSettings(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : 
	DemoSettingsBase(parent, id, title, pos, size, style) {
}

DemoSettings::~DemoSettings() {
}

void DemoSettings::OnInit(wxInitDialogEvent& event) {
	// Save the original settings in case user changes values, hits apply, but then cancels...
	originalBooleanValue = g_someBooleanValue;
	originalIntegerValue = g_someIntegerValue;
	originalStringValue = g_someStringValue;

	checkBoxBoolean->SetValue(g_someBooleanValue);
	sliderInteger->SetValue(g_someIntegerValue);
	textString->SetValue(g_someStringValue);

	Layout();
	Fit();

}

void DemoSettings::OnOK(wxCommandEvent& event) {
	g_someBooleanValue = checkBoxBoolean->IsChecked();
	g_someIntegerValue = sliderInteger->GetValue();
	g_someStringValue = textString->GetValue();
	EndModal(wxID_OK);
}

void DemoSettings::OnCancel(wxCommandEvent& event) {
	// Restore the original settings
	g_someBooleanValue = originalBooleanValue;
	g_someIntegerValue = originalIntegerValue;
	g_someStringValue = originalStringValue;
	EndModal(wxID_CANCEL);
}

void DemoSettings::OnApply(wxCommandEvent& event) {
	g_someBooleanValue = checkBoxBoolean->IsChecked();
	g_someIntegerValue = sliderInteger->GetValue();
	g_someStringValue = textString->GetValue();
}