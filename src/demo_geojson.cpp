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
// Description: Parse GeoJSON formatted file for OpenCPN Chart Plugin API's
// Owner: twocanplugin@hotmail.com
// Date: 10/01/2026
// Version History: 
// 1.0 Initial Release


#include "demo_geojson.h"

GeoJsonDecoder::GeoJsonDecoder() {
   // Nothing to do
}

GeoJsonDecoder::~GeoJsonDecoder() {
     // Nothing to do
}

// Load a GeoJson formatted file. Invoked at Plugin Chart Init
bool GeoJsonDecoder::Load(const wxString& filename) {

     wxFileInputStream inputStream(filename);

    if (!inputStream.IsOk()) {
        wxLogMessage("Demo Plugin, Error loading file: %s", filename);
        return false;
    }

    wxJSONValue root;
    wxJSONReader reader;

    int errors = reader.Parse(inputStream, &root);
    if (errors > 0) {
        wxLogMessage("Demo Plugin, GeoJSON Parsing error in %s", filename);
        wxArrayString jsonErrors = reader.GetErrors();
        for (auto it : jsonErrors) {
            wxLogMessage(it);
        }
        return false;
    }

    if (root["type"].AsString() != "FeatureCollection") {
        wxLogMessage("Demo Plugin, %s does not contain a GeoJSON feature collection", filename);
        return false;
    }

     wxJSONValue& features = root["features"];

    for (int i = 0; i < features.Size(); ++i) {
        ParseFeature(features[i]);
    }

    wxLogMessage("Demo Plugin, GeoJSON file %s contains %d features", filename, features.Size());
    return true;
}

// Getter
std::vector<GeoFeature>& GeoJsonDecoder::GetFeatures() {
    return featureCollection;
}

// Parse a minimal number of GeoJson Geometries, just polygons and lines
bool GeoJsonDecoder::ParseFeature( wxJSONValue& feature) {
    GeoFeature geometryFeature;

    // Iterate the properties
     wxJSONValue& props = feature["properties"];

     if (props.HasMember("featurecla")) {
         geometryFeature.featureClass = props["featurecla"].AsString();
     }

     if (props.HasMember("scalerank")) {
         geometryFeature.scaleRank = props["scalerank"].AsInt();
     }

     if (props.HasMember("min_zoom")) {
         geometryFeature.minZoom = props["min_zoom"].AsDouble();
     }

     if (props.HasMember("layer")) {
         geometryFeature.layer = props["layer"].AsString();
     }

    // Parse the various geometries
    wxJSONValue& geom = feature["geometry"];

    geometryFeature.geometryType = geom["type"].AsString();

    wxJSONValue& coords = geom["coordinates"];

    if (geometryFeature.geometryType == "Point") {
         GeoPoint p;
         p.x = coords[0].AsDouble();
         p.y = coords[1].AsDouble();

         geometryFeature.geoPoints.push_back({ p });
     }

    else if (geometryFeature.geometryType == "LineString") {
        std::vector<GeoPoint> line;

        for (int i = 0; i < coords.Size(); ++i) {
            GeoPoint p;
            p.x = coords[i][0].AsDouble();
            p.y = coords[i][1].AsDouble();
            line.push_back(p);
        }

        geometryFeature.geoPoints.push_back(std::move(line));
    }

    else if (geometryFeature.geometryType == "Polygon")  {
        for (int ringIdx = 0; ringIdx < coords.Size(); ++ringIdx) {
            std::vector<GeoPoint> ring;

             wxJSONValue& ringCoords = coords[ringIdx];

            for (int ptIdx = 0; ptIdx < ringCoords.Size(); ++ptIdx) {
                GeoPoint p;
                p.x = ringCoords[ptIdx][0].AsDouble();
                p.y = ringCoords[ptIdx][1].AsDouble();

                if (p.x > 180.0)
                    p.x -= 360.0;   // normalize to [-180, 180]


                ring.push_back(p);
            }

            geometryFeature.geoPoints.push_back(std::move(ring));
        }
    }

    else if (geometryFeature.geometryType == "MultiLineString") {
        for (int lineIdx = 0; lineIdx < coords.Size(); ++lineIdx) {
            std::vector<GeoPoint> line;
            wxJSONValue& lineCoords = coords[lineIdx];

            for (int ptIdx = 0; ptIdx < lineCoords.Size(); ++ptIdx) {
                GeoPoint p;
                p.x = lineCoords[ptIdx][0].AsDouble();
                p.y = lineCoords[ptIdx][1].AsDouble();
                line.push_back(p);
            }

            geometryFeature.geoPoints.push_back(std::move(line));
        }
    }

    else if (geometryFeature.geometryType == "MultiPolygon") {
        // coords = [ polygon0, polygon1, ... ]
        for (int polyIdx = 0; polyIdx < coords.Size(); ++polyIdx) {
            wxJSONValue& polygon = coords[polyIdx];

            // polygon = [ ring0, ring1, ... ]
            for (int ringIdx = 0; ringIdx < polygon.Size(); ++ringIdx) {
                wxJSONValue& ringCoords = polygon[ringIdx];
                std::vector<GeoPoint> ring;

                // ringCoords = [ [x,y], [x,y], ... ]
                for (int ptIdx = 0; ptIdx < ringCoords.Size(); ++ptIdx) {
                    GeoPoint p;
                    p.x = ringCoords[ptIdx][0].AsDouble();
                    p.y = ringCoords[ptIdx][1].AsDouble();
                    ring.push_back(p);
                }
                geometryFeature.geoPoints.push_back(std::move(ring));
            }
        }
    }
    
    else {
        wxLogMessage("Demo Plugin, GeoJSON file contains an unsupported geometry type: %s", geometryFeature.geometryType);
    }

    featureCollection.push_back(std::move(geometryFeature));

    return true;
}

// Determine the extents of the "chart"
// BUG BUG Do I need to normalize if the chart extents across meridian?
GeoBounds GeoJsonDecoder::CalculateBounds(const std::vector<GeoFeature>& features) {
    GeoBounds b;
    b.min.y = 90.0;
    b.max.y = -90.0;
    b.min.x = 180.0;
    b.max.x = -180.0;
    
    auto update = [&](const GeoPoint& p) {
        b.min.x = std::min(b.min.x, p.x);
        b.max.x = std::max(b.max.x, p.x);
        b.min.y = std::min(b.min.y, p.y);
        b.max.y = std::max(b.max.y, p.y);
        };

    for (const auto& f : features) {
        for (const auto& part : f.geoPoints) {
            for (const auto& p : part) {
                update(p);
            }
        }
    }

    return b; 
}


