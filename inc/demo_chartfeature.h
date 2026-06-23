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

#ifndef DEMO_CHART_FEATURE_H
#define DEMO_CHART_FEATURE_H

#include <vector>

#include <wx/string.h>

struct GeoPoint {
    double x = 0.0;
    double y = 0.0;
};

struct GeoFeature {
    wxString featureClass;
    int scaleRank = 0;
    double minZoom = 0.0;
    wxString layer;

    wxString geometryType;   // "Point", "LineString", "MultiLineString", "Polygon"

    // Universal geometry container:
    // - Point:        parts.size() == 1, parts[0].size() == 1
    // - LineString:   parts.size() == 1, parts[0] = list of points
    // - MultiLine:    parts.size() == N, each part = line
    // - Polygon:      parts.size() == rings, each ring = list of points
    std::vector<std::vector<GeoPoint>> geoPoints;
};



// Bounding Box
struct GeoBounds {
    GeoPoint min;
    GeoPoint max;
};

#endif
