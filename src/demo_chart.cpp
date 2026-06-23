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
// Description: Demonstrate OpenCPN Plugin Chart API's
// Owner: twocanplugin@hotmail.com
// Date: 20/06/2026
// Version History: 
// 1.0 Initial Release

#include "demo_chart.h"

// A decoder that parses geoJSON formatted text files
#include "demo_geojson.h"

// OpenCPN uses wxWidgets Run Time Type Information (RTTI) to 
// dynamically load chart plugins. There is no ChartPlugin factory
IMPLEMENT_DYNAMIC_CLASS(DemoChart, PlugInChartBaseGL)

// Constructor and destructor implementation
DemoChart::DemoChart() : PlugInChartBase() {

}

DemoChart::~DemoChart() {

}

// Return a string representing the chart file extensions this plugin supports
// For this demo, copy the file named Oz.abc in the plugin data directory
// to another folder, then add that folder to the list of OpenCPN Chart folders.
wxString DemoChart::GetFileSearchMask(void) {

    return "*.abc";
}
// Called by OpenCPN when initializing a chart
// Eg. When adding new chart folder or rebuilding the chart database.
int DemoChart::Init(const wxString& full_path, int init_flags) {

    if (!wxFileName::FileExists(full_path)) {
        wxLogMessage("Demo Chart Plugin, GeoJSON chart file not found: %s", full_path);
        return PI_INIT_FAIL_REMOVE;
    }

    // Instantiate the demo GeoJSON parser
    m_jsonDecoder = std::make_unique<GeoJsonDecoder>();

    // Persist the name of the chart file (used later on for chart metadata)
    m_chartPath = full_path;

    // If we fail to open/parse the file, remove this from the OpenCPN chart database
    if (!m_jsonDecoder->Load(full_path)) {
        wxLogMessage("Demo Chart Plugin, GeoJSON chart failed to load: %s", full_path);
        return PI_INIT_FAIL_REMOVE;
    }

    // Parse all of the features
    m_chartFeatures = m_jsonDecoder->GetFeatures();

    // Zoom level is present in the first record of the demo geojson file
    // Eg. "features":  [ { "type":  "Feature", "properties" : {...... "min_zoom" : 1.500000 }}]
    m_Chart_Scale = m_chartFeatures.at(0).minZoom;

    // The chart bounds are important, it is how OpenCPN determines whether to 
    // display this chart. (Eg. Does the chart lie within the viewport?)
    m_chartBounds = m_jsonDecoder->CalculateBounds(m_chartFeatures);

    // Build a table of polygons (pairs of doubles) that describe the coverage areas
    BuildCoverageFromFeatures();

    return PI_INIT_OK;
}

// Change our chart colours for day/dusk/night
void DemoChart::SetColorScheme(int cs, bool bApplyImmediate) {
    // Don't worry about colour scheme yet
}

// These seem to work for charts derived from Natural Earth 1:50M
double DemoChart::GetNormalScaleMin(double canvas_scale_factor, bool b_allow_overzoom) {
    return m_Chart_Scale * (b_allow_overzoom ? 0.125 : 0.25);;
}

double DemoChart::GetNormalScaleMax(double canvas_scale_factor, int canvas_width) {
    return m_Chart_Scale * 4.0;
}

double DemoChart::GetNearestPreferredScalePPM(double target_scale_ppm) {
    return target_scale_ppm;
}

// Determined when parsing the chart
bool DemoChart::GetChartExtent(ExtentPI* pext) {
    pext->ELON = m_chartBounds.max.x;
    pext->WLON = m_chartBounds.min.x;
    pext->NLAT = m_chartBounds.max.y;
    pext->SLAT = m_chartBounds.min.y;

    return true;
}

// This plugin doesn't care when the viewport is adjusted. 
// Nonetheless, returnung false seems to have no effect
bool DemoChart::AdjustVP(PlugIn_ViewPort& vp_last, PlugIn_ViewPort& vp_proposed) {
   return true;
}

// Mark the whole viewport as valid for repainting
void DemoChart::GetValidCanvasRegion(const PlugIn_ViewPort& VPoint, wxRegion* pValidRegion) {
    if (!pValidRegion) {
        return;
    }
    else {
        pValidRegion->Clear();
        pValidRegion->Union(0, 0, VPoint.pix_width, VPoint.pix_height);
    }
}

// Return the number of entries (polygons) describing the coverage area
int DemoChart::GetCOVREntries() {
    return (int)m_coverageTable.size();
}

// Return a the number of value pairs for each entry that describe the coverage area.
int DemoChart::GetCOVRTablePoints(int iTable) {
    if (iTable < 0 || iTable >= (int)m_coverageTable.size()) {
        return 0;
    }
    else {
        return (int)m_coverageTable[iTable].size() / 2; // lat/lon pairs
    }
}

// Return a pointer to the value pairs for each entry that describe the coverage area.
float* DemoChart::GetCOVRTableHead(int iTable) {
    if (iTable < 0 || iTable >= (int)m_coverageTable.size()) {
        return nullptr;
    }
    return m_coverageTable[iTable].data();
}

// Generate a vector of pairs of floats for all of the coverage areas (i.e polygons)
// I think it is used by OpenCPN for displaying chart outlines
// Refer to GetCOVREntries, GetCOVRTablePoints, GetCOVRTableHead above.
void DemoChart::BuildCoverageFromFeatures() {

    m_coverageTable.clear();

    for (const auto& feature : m_chartFeatures) {
        if (feature.geometryType == "Polygon" || feature.geometryType == "MultiPolygon") {
            for (const auto& polygon : feature.geoPoints) {
                std::vector<float> coveragePoints;
                coveragePoints.reserve(polygon.size() * 2);
                // Fucking GeoJSON stores points in the reverse order; longitude, latitude.
                // Swap them around, otherwise OpenCPN will display the chart outline incorrectly
                for (const auto& point : polygon) {
                    coveragePoints.push_back((float)point.y);
                    coveragePoints.push_back((float)point.x);
                }
                m_coverageTable.push_back(std::move(coveragePoints));
            }
        }
    }
}


// No idea when this is called? Just return a red coloured bitmap.
wxBitmap* DemoChart::GetThumbnail(int tnx, int tny, int cs) {
    bitMap.Create(tnx, tny, 32);
    wxMemoryDC dc;
    dc.SelectObject(bitMap);
    dc.Clear();
    dc.SetBackground(wxColour(255, 0, 0, 40));
    dc.SelectObject(wxNullBitmap);
    return &bitMap;
}

// The following are chart metadata. Observed when hovering over the "piano" key
// In reality these should be obtained when parsing a "proper" chart and persisted as member variables

wxString DemoChart::GetFullPath() const {
    return m_chartPath;
}

ChartTypeEnumPI DemoChart::GetChartType() {
    return ChartTypeEnumPI::PI_CHART_TYPE_PLUGIN;
}

// This demo uses GeoJSON formatted charts, therefore these are vector charts
ChartFamilyEnumPI DemoChart::GetChartFamily() {
    return ChartFamilyEnumPI::PI_CHART_FAMILY_VECTOR;
}

// I don't believe the Natural Earth GEO JSON files use Mercator projection?
OcpnProjTypePI DemoChart::GetChartProjection() {
    return OcpnProjTypePI::PI_PROJECTION_EQUIRECTANGULAR;
}

wxString DemoChart::GetName() {
    return "Demo Chart";
}

wxString DemoChart::GetDescription() {
    return "Demo Chart Description";
}

wxString DemoChart::GetID() {
    return "Some Id";
}

wxString DemoChart::GetSE() {
    return "Demo Chart Authority";
}

wxString DemoChart::GetDepthUnits() {
    return "Metres";
}

wxString DemoChart::GetSoundingsDatum() {
    return "LAT";
}

wxString DemoChart::GetDatumString() {
    return "WGS84";
}

wxString DemoChart::GetExtraInfo() {
    return "Extra Info: Not to be used for navigation";
}

wxString DemoChart::GetPubDate() {
    return wxDateTime::Now().FormatISOCombined();
}

double DemoChart::GetChartErrorFactor() {
    return 0.0;
}

ChartDepthUnitTypePI DemoChart::GetDepthUnitId() {
    return ChartDepthUnitTypePI::PI_DEPTH_UNIT_METERS;
}

// Should derive from the geojson file, the sample file was derived from a 1:50M chart
int DemoChart::GetNativeScale() {
    return 50000000;
}

double DemoChart::GetChartSkew() {
    return 0.0;
}

wxDateTime DemoChart::GetEditionDate(void) {
    return wxDateTime::Now();
}

// Rendering routines 
// If the plugin had a long running operation, return as appropriate
bool DemoChart::IsReadyToRender() {
    return true;
}

// Actually render the chart
wxBitmap& DemoChart::RenderRegionView(const PlugIn_ViewPort& vp, const wxRegion& Region) {
    //auto start = std::chrono::high_resolution_clock::now();

    PlugIn_ViewPort viewPort = vp;

    // Lambda function to set the Pen & Brush colours depending on the layer
    auto setColour = [](wxMemoryDC& dc, wxString layer) {
        if (layer == "land") {
            dc.SetPen(wxColour(204, 204, 0, 255));
            dc.SetBrush(wxColour(204, 204, 0, 40));
        }
        else if (layer == "river") {
            dc.SetPen(wxColour(40, 115, 230, 255));
            dc.SetBrush(wxColour(40, 115, 230, 40));
        }
        else if (layer == "lake") {
            dc.SetPen(wxColour(25, 75, 190, 255));
            dc.SetBrush(wxColour(25, 75, 190, 40));
        }
        else if (layer == "coastline") {
            dc.SetPen(wxColour(240, 230, 0, 255));
            dc.SetBrush(wxColour(240, 230, 0, 40));
        }
        else {
            dc.SetPen(wxColour(255, 255, 255, 255));
            dc.SetBrush(wxColour(255, 255, 255, 40));
        }
    };

    // Create a bitmap sized to the viewport
    bitMap.Create(viewPort.pix_width, viewPort.pix_height, 32);

    if (bitMap.IsOk()) {
        wxMemoryDC dc(bitMap);
        dc.SelectObject(bitMap);

        dc.Clear();

        // Sky blue background
        dc.SetBackground(wxColour(135, 206, 250, 40));

        for (const auto& chartFeature : m_chartFeatures) {

            setColour(dc, chartFeature.layer);

            if (chartFeature.geometryType == "Polygon" || chartFeature.geometryType == "MultiPolygon") {

                for (const auto& polygon : chartFeature.geoPoints) {

                    // Convert to screen co-ordinates, would be nice to use a transform ??
                    // If we were using C++20, wonder if any benefit from using std::execution::par ??
                    std::vector<wxPoint> screenPoints(polygon.size());
                    std::transform(polygon.begin(), polygon.end(),screenPoints.begin(),
                        [viewPort](const auto& geoPoint) {
                            wxPoint point;
                            GetCanvasPixLL((PlugIn_ViewPort*)&viewPort, &point, geoPoint.y, geoPoint.x);
                            return point;
                        });

                    // Replaced by above....but is it any better ??
                    // for (size_t i = 0; i < polygon.size(); ++i) {
                    //    GetCanvasPixLL((PlugIn_ViewPort*)&viewPort, &screenPoints[i], polygon[i].y, polygon[i].x);
                    // }

                    dc.DrawPolygon(screenPoints.size(), screenPoints.data());
                }
            }
            else if (chartFeature.geometryType == "LineString" || chartFeature.geometryType == "MultiLineString") {
                for (const auto& line : chartFeature.geoPoints) {
                    for (size_t i = 1; i < line.size(); ++i) {
                        wxPoint p1, p2;
                        GetCanvasPixLL((PlugIn_ViewPort*)&viewPort, &p1, line[i - 1].y, line[i - 1].x);
                        GetCanvasPixLL((PlugIn_ViewPort*)&viewPort, &p2, line[i].y, line[i].x);
                        dc.DrawLine(p1, p2);
                    }
                }
            }
        }

        // Return the bitmap to OpenCPN
        dc.SelectObject(wxNullBitmap);
    }
    else {
        wxLogMessage("Demo Plugin, Chart Render Error, Bitmap is not OK");
        bitMap = wxBitmap();
    }

   // auto end = std::chrono::high_resolution_clock::now();
   // wxLogMessage("XXXXXX Duration: %d", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    return bitMap;
}