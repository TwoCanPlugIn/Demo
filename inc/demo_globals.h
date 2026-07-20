
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

#ifndef DEMO_GLOBALS_H
#define DEMO_GLOBALS_H

// wxWidgets Precompiled Headers
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif

enum class Category {
	Marina,
	Anchorage,
	Hazard,
	Business,
	BoatRamp,
	Bridge,
	Dam,
	Ferry,
	Inlet,
	Lock,
	Default
};

static const std::unordered_map<wxString, Category> categoryMap = {
	{"Marina",    Category::Marina},
	{"Anchorage", Category::Anchorage},
	{"Hazard",    Category::Hazard},
	{"Business",  Category::Business},
	{"BoatRamp",  Category::BoatRamp},
	{"Bridge",    Category::Bridge},
	{"Dam",       Category::Dam},
	{"Ferry",     Category::Ferry},
	{"Inlet",     Category::Inlet},
	{"Lock",      Category::Lock}
};


struct CategoryInfo {
	Category category;
	wxString name;
};

extern std::vector<CategoryInfo> categories;

// Hold information about an Active Captain Point of Interest
struct ActiveCaptainPOI {
	wxString id;
	wxString name;
	Category type;
	double latitude;
	double longitude;
	double averageRating;
	int reviewCount;
	wxString notes;
};

// Globally accessible variables used by the plugin, dialogs etc.

extern wxBitmap pluginBitmap;
extern wxBitmap toolbarBitmap;
// POI bitmaps (Converted from Garmin POI SVG files in theActive Caotain SDK)
extern wxBitmap bitmapMarina;
extern wxBitmap bitmapAnchorage;
extern wxBitmap bitmapHazard;
extern wxBitmap bitmapBusiness;
extern wxBitmap bitmapBoatRamp;
extern wxBitmap bitmapBridge;
extern wxBitmap bitmapDam;
extern wxBitmap bitmapFerry;
extern wxBitmap bitmapInlet;
extern wxBitmap bitmapLock;
extern wxBitmap bitmapDefault;

//The different Garmin Active Captain Points of Interest
extern bool g_showMarina;
extern bool g_showAnchorage;
extern bool g_showHazard;
extern bool g_showBusiness;
extern bool g_showBoatRamp;
extern bool g_showBridge;
extern bool g_showDam;
extern bool g_showFerry;
extern bool g_showInlet;
extern bool g_showLock;

// Id's used for wxWidgets thingies
constexpr int k_GarminContextMenu = wxID_HIGHEST + 1;

#endif
