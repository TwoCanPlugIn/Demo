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

#ifndef DEMO_CHARTGL_H
#define DEMO_CHARTGL_H

// wxWidgets Precompiled Headers
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif

// for OpenGL Tesselator
#ifdef __WXMSW__
#include <windows.h>   // must come before gl/gl.h on Windows
#endif
#include <GL/gl.h>
#include <GL/glu.h>


// The chart plugin uses some of the OpenCPN functions for chart display
#include "ocpn_plugin.h"

// Implements the decoder to parse geojson formatted files
#include "demo_geojson.h"

#include <wx/string.h>
#include <wx/filename.h>
#include <wx/log.h>

#include <chrono>
#include <vector>

// OpenCPN Plugin Chart demo. Note the Class Name is required for the Plugin Callback
// API, GetDynamicChartClassNameArray()
class DemoChartGL : public PlugInChartBaseGL {

// OpenCPN uses RTTI to load the Chart Plugin
DECLARE_DYNAMIC_CLASS(DemoChartGL)

public:
    DemoChartGL();
    ~DemoChartGL();
    wxBitmap bitMap;

    // Overridden OpenCPN PluginInChart methods
    wxString GetFileSearchMask(void) override;
    int Init(const wxString& full_path, int init_flags) override;
    void SetColorScheme(int cs, bool bApplyImmediate) override;
    double GetNormalScaleMin(double canvas_scale_factor, bool b_allow_overzoom) override;
    double GetNormalScaleMax(double canvas_scale_factor, int canvas_width) override;
    double GetNearestPreferredScalePPM(double target_scale_ppm) override;
    bool GetChartExtent(ExtentPI* pext) override;
    bool AdjustVP(PlugIn_ViewPort& vp_last, PlugIn_ViewPort& vp_proposed) override;
    void GetValidCanvasRegion(const PlugIn_ViewPort& VPoint, wxRegion* pValidRegion) override;
    int GetCOVREntries() override;
    int GetCOVRTablePoints(int iTable) override;
    float* GetCOVRTableHead(int iTable) override;
    wxBitmap* GetThumbnail(int tnx, int tny, int cs) override;
    wxString GetFullPath() const override;
    ChartTypeEnumPI GetChartType() override;
    ChartFamilyEnumPI GetChartFamily() override;
    wxString GetName() override;
    wxString GetDescription() override;
    wxString GetID() override;
    wxString GetSE() override;
    wxString GetDepthUnits() override;
    wxString GetSoundingsDatum() override;
    wxString GetDatumString() override;
    wxString GetExtraInfo() override;
    wxString GetPubDate() override;
    OcpnProjTypePI GetChartProjection() override;
    double GetChartErrorFactor() override;
    ChartDepthUnitTypePI GetDepthUnitId() override;
    bool IsReadyToRender() override;
    int GetNativeScale() override;
    double GetChartSkew() override;
    wxDateTime GetEditionDate(void) override;

    // ChartBaseGL overridden methods
    int RenderRegionViewOnGL(const wxGLContext& glc, const PlugIn_ViewPort& VPoint,
        const wxRegion& Region, bool b_use_stencil) override;
    int GetNoCOVREntries() override;
    int GetNoCOVRTablePoints(int iTable) override;
    int GetNoCOVRTablenPoints(int iTable) override;
    float* GetNoCOVRTableHead(int iTable) override;

protected:

private:
    // The demo class that parses a GeoJSON formatted text file
    std::unique_ptr<GeoJsonDecoder> m_jsonDecoder;
    
    // The path to the chart file
    wxString m_chartPath;

    // All of the features (lines, polygons, as parsed by the decoder)
    std::vector<GeoFeature> m_chartFeatures;

    // The chart extents given as minimum and maximum latitude & longitude
    GeoBounds m_chartBounds;

    // The polygons that define the chart coverage
    std::vector<std::vector<float>> m_coverageTable;

    // Constructs the above table
    void BuildCoverageFromFeatures();

};

#endif
