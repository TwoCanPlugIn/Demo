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

#ifndef DEMO_GEOJSON_H
#define DEMO_GEOJSON_H

// wxWidgets Precompiled Headers
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/log.h>

#include <wx/jsonreader.h>
#include <wx/jsonval.h>
#include <vector>

#include "demo_chartfeature.h"

class GeoJsonDecoder {
public:
    GeoJsonDecoder();
    ~GeoJsonDecoder();
    bool Load(const wxString& filename);
    std::vector<GeoFeature>& GetFeatures(); 
    GeoBounds CalculateBounds(const std::vector<GeoFeature>& features);

private:
    bool ParseFeature(wxJSONValue& feature);
    std::vector<GeoFeature> featureCollection;
};

#endif
