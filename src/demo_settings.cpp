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
// 1.0.1, 28/04/2026 - Updated for Active Captain POC

#include "demo_settings.h"

// Constructor and destructor implementation
DemoSettings::DemoSettings(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : 
	DemoSettingsBase(parent, id, title, pos, size, style) {
}

DemoSettings::~DemoSettings() {
}

void DemoSettings::OnInit(wxInitDialogEvent& event) {

	checkMarina->SetValue(g_showMarina);
	checkAnchorage->SetValue(g_showAnchorage);
	checkHazard->SetValue(g_showHazard);
	checkBusiness->SetValue(g_showBusiness);
	checkBoatRamp->SetValue(g_showBoatRamp);
	checkBridge->SetValue(g_showBridge);
	checkDam->SetValue(g_showDam);
	checkFerry->SetValue(g_showFerry);
	checkLock->SetValue(g_showLock);
	bitmapGarmin->SetBitmap(pluginBitmap);
	labelDisclaimer->SetLabel("Active Captain\xae and Garmin\xae are registered trademarks of Garmin Ltd.\nThis plugin is not developed, endorsed or supported by Garmin.");
	SetMinSize(wxSize(300, 400));
	Layout();
	Fit();
	CenterOnParent();

}

void DemoSettings::OnOK(wxCommandEvent& event) {
	g_showMarina = checkMarina->IsChecked();
	g_showAnchorage = checkAnchorage->IsChecked();
	g_showHazard = checkHazard->IsChecked();
	g_showBusiness = checkBusiness->IsChecked();
	g_showBoatRamp = checkBoatRamp->IsChecked();
	g_showBridge = checkBridge->IsChecked();
	g_showDam = checkDam->IsChecked();
	g_showFerry = checkFerry->IsChecked();
	g_showLock = checkLock->IsChecked();
	EndModal(wxID_OK);
}

void DemoSettings::OnCancel(wxCommandEvent& event) {
	EndModal(wxID_CANCEL);
}