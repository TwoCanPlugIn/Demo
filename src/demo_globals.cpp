// Copyright(C) 2018-2025 by Steven Adler
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
// Description: Demonstrate the use of the OpenCPN plugin API's
// Owner: twocanplugin@hotmail.com
// Date: 10/01/2026
// Version History: 
// 1.0 Initial Release
// 1.0.1, 28/04/2026 - Updated for Active Captain POC

#include "demo_globals.h"

std::vector<CategoryInfo> categories = {
	{Category::Marina,    wxString::FromAscii("Marina")},
	{Category::Anchorage, wxString::FromAscii("Anchorage")},
	{Category::Hazard,    wxString::FromAscii("Hazard")},
	{Category::Business,  wxString::FromAscii("Business")},
	{Category::BoatRamp,  wxString::FromAscii("BoatRamp")},
	{Category::Bridge,    wxString::FromAscii("Bridge")},
	{Category::Dam,       wxString::FromAscii("Dam")},
	{Category::Ferry,     wxString::FromAscii("Ferry")},
	{Category::Inlet,     wxString::FromAscii("Inlet")},
	{Category::Lock,      wxString::FromAscii("Lock")}
};

// Globally accessible variables used by the plugin, dialogs etc.

// Plugin icon
wxBitmap pluginBitmap;
wxBitmap toolbarBitmap;
wxBitmap bitmapMarina;
wxBitmap bitmapAnchorage;
wxBitmap bitmapHazard;
wxBitmap bitmapBusiness;
wxBitmap bitmapBoatRamp;
wxBitmap bitmapBridge;
wxBitmap bitmapDam;
wxBitmap bitmapFerry;
wxBitmap bitmapInlet;
wxBitmap bitmapLock;
wxBitmap bitmapDefault;

// The different Garmin Active Captain Points of Interest
bool g_showMarina;
bool g_showAnchorage;
bool g_showHazard;
bool g_showBusiness;
bool g_showBoatRamp;
bool g_showBridge;
bool g_showDam;
bool g_showFerry;
bool g_showInlet;
bool g_showLock;