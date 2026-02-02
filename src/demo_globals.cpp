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

#include "demo_globals.h"

// Globally accessible variables used by the plugin, dialogs etc.

// Plugin icon
wxBitmap g_pluginBitmap;

// Some fictitious plugin settings
bool g_someBooleanValue = false;
int g_someIntegerValue = 0;
wxString g_someStringValue = wxEmptyString;