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

//
// Project: Demo Plugin
// Description: a wxWidgets wxPanel added to the OpenCPN Toolbox to modify settings
// Owner: twocanplugin@hotmail.com
// Date: 10/01/2026
// Version History: 
// 1.0 Initial Release
// 1.0.1, 28/04/2026 - Updated for Active Captain POC

#include "demo_toolbox.h"

#include "demo_globals.h"

// Constructor and destructor implementation
DemoToolbox::DemoToolbox( wxWindow* parent) : DemoToolboxBase(parent) {
	
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
}

DemoToolbox::~DemoToolbox() {
}