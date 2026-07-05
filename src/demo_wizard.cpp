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
// Project: Demo Plugin
// Description: A wxWidgets wxWizard used to initially configure the plugin when installed/enabled
// Owner: twocanplugin@hotmail.com
// Date: 10/01/2026
// Version History: 
// 1.0 Initial Release

#include "demo_wizard.h"

#include "demo_globals.h"

// Constructor and destructor implementation
DemoWizard::DemoWizard(wxWindow* parent) : DemoWizardBase(parent) {
}

DemoWizard::~DemoWizard() {
}

void DemoWizard::OnInit(wxInitDialogEvent& event) {
	checkBoxBoolean->SetValue(false);
	sliderInteger->SetValue(50);
	textString->SetValue("Demo Plugin");

	// For this demo, we need a vessel name to export to RTZ and S421
	labelString->SetLabel("Vessel Name");

	Layout();
	Fit();

}

void DemoWizard::OnCancel(wxWizardEvent& event) {
	g_someBooleanValue = false;
	g_someIntegerValue = 0;
	g_someStringValue = wxEmptyString;
}

void DemoWizard::OnFinished(wxWizardEvent& event) {
	g_someBooleanValue = checkBoxBoolean->IsChecked();
	g_someIntegerValue = sliderInteger->GetValue();
	g_someStringValue = textString->GetValue();
}