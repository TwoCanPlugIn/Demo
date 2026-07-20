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
// 1.0.1, 28/04/2026 - Updated for Active Captain POC

#include "demo_wizard.h"

#include "demo_globals.h"

// Constructor and destructor implementation
DemoWizard::DemoWizard(wxWindow* parent) : DemoWizardBase(parent) {
}

DemoWizard::~DemoWizard() {
}

void DemoWizard::OnInit(wxInitDialogEvent& event) {
	
	checkMarina->SetValue(false);
	checkAnchorage->SetValue(false);
	checkHazard->SetValue(false);
	checkBusiness->SetValue(false);
	checkBoatRamp->SetValue(false);
	checkBridge->SetValue(false);
	checkDam->SetValue(false);
	checkFerry->SetValue(false);
	checkLock->SetValue(false);
	bitmapGarmin->SetBitmap(pluginBitmap);
	labelDisclaimer->SetLabel("Active Captain\xae and Garmin\xae are registered trademarks of Garmin Ltd.\nThis plugin is not developed, endorsed or supported by Garmin.");
	Layout();
	Fit();
}

void DemoWizard::OnCancel(wxWizardEvent& event) {
	
}

void DemoWizard::OnFinished(wxWizardEvent& event) {
	g_showMarina = checkMarina->IsChecked();
	g_showAnchorage = checkAnchorage->IsChecked();
	g_showHazard = checkHazard->IsChecked();
	g_showBusiness = checkBusiness->IsChecked();
	g_showBoatRamp = checkBoatRamp->IsChecked();
	g_showBridge = checkBridge->IsChecked();
	g_showDam = checkDam->IsChecked();
	g_showFerry = checkFerry->IsChecked();
	g_showLock = checkLock->IsChecked();
}