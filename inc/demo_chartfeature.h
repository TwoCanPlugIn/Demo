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
#include <algorithm>

#include <wx/string.h>

// Chart Point
struct GeoPoint {
    double x = 0.0;
    double y = 0.0;
};

// Chart Feature
struct GeoFeature {
    wxString featureClass;
    int scaleRank = 0;
    double minZoom = 0.0;
    wxString layer; // "land", "river", "lake"

    wxString geometryType; // "Point", "LineString", "MultiLineString", "Polygon"

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

    bool Contains(const GeoPoint& p) const {
        return p.x >= min.x && p.x <= max.x &&
            p.y >= min.y && p.y <= max.y;
    }

    // True if this box fully contains other (used to decide which single
    // child a feature's bbox can be pushed down into).
    bool ContainsBounds(const GeoBounds& other) const {
        return other.min.x >= min.x && other.max.x <= max.x &&
            other.min.y >= min.y && other.max.y <= max.y;
    }

    bool Intersects(const GeoBounds& other) const {
        return !(other.min.x > max.x || other.max.x < min.x ||
            other.min.y > max.y || other.max.y < min.y);
    }
};

inline GeoBounds ComputeBounds(const std::vector<std::vector<GeoPoint>>& parts) {
    GeoBounds b;
    b.min = { std::numeric_limits<double>::max(),    std::numeric_limits<double>::max() };
    b.max = { std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest() };

    for (const auto& part : parts) {
        for (const auto& p : part) {
            b.min.x = std::min(b.min.x, p.x);
            b.min.y = std::min(b.min.y, p.y);
            b.max.x = std::max(b.max.x, p.x);
            b.max.y = std::max(b.max.y, p.y);
        }
    }
    return b;
}



// Chart details
struct ChartInformation {
    wxString fileName;
    GeoBounds chartBounds;
};

#endif
