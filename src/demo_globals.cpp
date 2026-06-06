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

// Project: Gateway Plugin
// Description: Derived from Demo plugin, a simple bi-directional NMEA 0183 <-> NMEA 2000 gateway
// Owner: twocanplugin@hotmail.com
// Date: 17/04/2026
// Version History: 
// 1.0 Initial Release

#include "demo_globals.h"

// Globally accessible variables used by the plugin, dialogs etc.

// Plugin icon
wxBitmap g_pluginBitmap;

// Persist the NMEA 0183 and NMEA 2000 network connections
wxString g_nmea0183Driver;
wxString g_nmea2000Driver;

// Settings for bi-directional NMEA 0183 & NMEA 2000 conversions
ConversionType g_Depth = ConversionType::None;
ConversionType g_Wind = ConversionType::None;
ConversionType g_Speed = ConversionType::None;
ConversionType g_Position = ConversionType::None;

// NMEA 2000 PGN's and NMEA 0183 Sentences and descriptions to populate the grid
std::unordered_map<int, std::string> supportedConversions = { 
    {130306, "MWV (Wind)"},
	{128267, "DPT (Depth)"},
    {128259, "VHW (Speed)"},
    {129025, "GLL (Position)"} };

// Map the PGN's to their respective global variables
std::unordered_map<int, ConversionType*> variableBindings = {
    {130306,  &g_Wind},
    {128267, &g_Depth},
    {128259, &g_Speed},
    {129025, &g_Position} };