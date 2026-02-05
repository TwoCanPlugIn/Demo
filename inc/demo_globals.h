
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

// Globally accessible variables used by the plugin, dialogs etc.

extern wxBitmap g_pluginBitmap;
extern bool g_someBooleanValue;
extern int g_someIntegerValue;
extern wxString g_someStringValue;

// Id's used for wxWidgets thingies
constexpr int k_FirstContextMenu = wxID_HIGHEST + 1;
constexpr int k_SecondContextMenu = wxID_HIGHEST + 2;

#endif