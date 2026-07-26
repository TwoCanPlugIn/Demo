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
// Date: 27/06/2026
// Version History: 
// 1.0 Initial Release

#include "demo_chartgl.h"

// Used to store memory allocations when the tessalator combines vertices which are later deallocated
std::vector<GLdouble*> temporaryVertices;

// The tesselator callbacks are how the OpenGL Utility Library (GLU) works

// Tessellator callback types
struct Vert3 { double x, y, z; };

struct TessContext {
    std::vector<std::array<double, 3>> vertices; // GLU requires arrays of 3 doubles. Eg. double[3]
};

static void CALLBACK tessVertex(void* data) {
    double* pt = static_cast<double*>(data);
    glVertex2d(pt[0], pt[1]);
}

static void CALLBACK tessBegin(GLenum type) {
    glBegin(type);
}

static void CALLBACK tessEnd() {
    glEnd();
}

static void CALLBACK tessError(GLenum err) {
    // Convert an ANSI error string
    wxLogMessage("Demo Chart Plugin, GLU tesselator error: %s", wxString(gluErrorString(err), wxConvISO8859_1));
}

static void CALLBACK tessCombine(GLdouble coords[3], void* vertex_data[4], GLfloat weight[4], void** outData) {
    // Allocate memory for the new vertex (must be freed later)
    GLdouble* newVertex = new GLdouble[3];

    // Save the memory location (Remember to free it!)
    temporaryVertices.push_back(newVertex);

    // Set the coordinates of the new vertex
    newVertex[0] = coords[0];
    newVertex[1] = coords[1];
    newVertex[2] = coords[2];

    // Note to self.

    // A----------B
    //  \        /
    //   \      /
    //    X    /
    //    /\  /
    //   /  \/
    //  C----D


    // Any other interpolation could be performed here, for example
    // using the weights array if using textured or colored geometry.

    // Pass the pointer for the new vertex to the tessellator
    *outData = newVertex;
}


// OpenCPN uses wxWidgets Run Time Type Information (RTTI) to 
// dynamically load chart plugins. There is no ChartPlugin factory
IMPLEMENT_DYNAMIC_CLASS(DemoChartGL, PlugInChartBaseGL)

// Constructor and destructor implementation
DemoChartGL::DemoChartGL() : PlugInChartBaseGL() {

    // Instantiate the demo GeoJSON parser
    m_jsonDecoder = std::make_unique<GeoJsonDecoder>();

    // Quick and Dirty Quadtree
    chartCache = std::make_unique<QuadTree>(GeoBounds{GeoPoint { -180.0, -90.0}, GeoPoint{180.0,90.0} });
}

DemoChartGL::~DemoChartGL() {

}

// Return a string representing the chart file extensions this plugin supports
// For this demo, copy the file named Oz.abc in the plugin data directory
// to another folder, then add that folder to the list of OpenCPN Chart folders.
wxString DemoChartGL::GetFileSearchMask(void) {

    return "*.abc";
}
// Called by OpenCPN when initializing a chart
// Eg. When adding new chart folder or rebuilding the chart database.
int DemoChartGL::Init(const wxString& full_path, int init_flags) {

    if (!wxFileName::FileExists(full_path)) {
        wxLogMessage("Demo Chart Plugin, GeoJSON chart file not found: %s", full_path);
        return PI_INIT_FAIL_REMOVE;
    }

    // Persist the name of the chart file (used later on for chart metadata)
    m_chartPath = full_path;

    // If we fail to open/parse the file, remove this from the OpenCPN chart database
    if (!m_jsonDecoder->Load(full_path)) {
        wxLogMessage("Demo Chart Plugin, GeoJSON chart failed to load: %s", full_path);
        return PI_INIT_FAIL_REMOVE;
    }

    // Parse all of the features
    m_chartFeatures = m_jsonDecoder->GetFeatures();

    // Add to the quadtree
    for (size_t i = 0; i < m_chartFeatures.size(); ++i) {
        GeoBounds b = ComputeBounds(m_chartFeatures[i].geoPoints);
        chartCache->Insert(FeatureReference{ b, i });
    }

    wxLogDebug("From %s, Read %d, Quadtree %d", m_chartPath, m_chartFeatures.size(), chartCache->Size());

    // Zoom level is present in the first record of the demo geojson file
    m_Chart_Scale = m_chartFeatures.at(0).minZoom;

    // The chart bounds are important, it is how OpenCPN determines whether to 
    // display this chart. (Eg. Does the chart lie within the viewport?)
    // BUG BUG duplicated ! See above
    m_chartBounds = m_jsonDecoder->CalculateBounds(m_chartFeatures);

    // Build a table of polygons (pairs of doubles) that describe the coverage areas
    BuildCoverageFromFeatures();

    return PI_INIT_OK;
}

// Change our chart colours for day/dusk/night
void DemoChartGL::SetColorScheme(int cs, bool bApplyImmediate) {
    // Don't worry about colour scheme yet
}

// These seem to work for charts derived from Natural Earth 1:50M
double DemoChartGL::GetNormalScaleMin(double canvas_scale_factor, bool b_allow_overzoom) {
    return m_Chart_Scale * (b_allow_overzoom ? 0.125 : 0.25);;
}

double DemoChartGL::GetNormalScaleMax(double canvas_scale_factor, int canvas_width) {
    return m_Chart_Scale * 4.0;
}

double DemoChartGL::GetNearestPreferredScalePPM(double target_scale_ppm) {
    return target_scale_ppm;
}

// Determined when parsing the chart
bool DemoChartGL::GetChartExtent(ExtentPI* pext) {
    pext->ELON = m_chartBounds.max.x;
    pext->WLON = m_chartBounds.min.x;
    pext->NLAT = m_chartBounds.max.y;
    pext->SLAT = m_chartBounds.min.y;

    return true;
}

// This plugin doesn't care when the viewport is adjusted
bool DemoChartGL::AdjustVP(PlugIn_ViewPort& vp_last, PlugIn_ViewPort& vp_proposed) {
    return true;
}

// ???
void DemoChartGL::GetValidCanvasRegion(const PlugIn_ViewPort& VPoint, wxRegion* pValidRegion) {
    if (!pValidRegion) {
        return;
    }
    else {
        pValidRegion->Clear();
        pValidRegion->Union(0, 0, VPoint.pix_width, VPoint.pix_height);
    }
}

// Return the number of entries (polygons) describing the coverage area
int DemoChartGL::GetCOVREntries() {
    return (int)m_coverageTable.size();
}

// Return a the number of value pairs for each entry that describe the coverage area.
int DemoChartGL::GetCOVRTablePoints(int iTable) {
    if (iTable < 0 || iTable >= (int)m_coverageTable.size()) {
        return 0;
    }
    else {
        return (int)m_coverageTable[iTable].size() / 2; // lat/lon pairs
    }
}

// Return a pointer to the value pairs for each entry that describe the coverage area.
float* DemoChartGL::GetCOVRTableHead(int iTable) {
    if (iTable < 0 || iTable >= (int)m_coverageTable.size()) {
        return nullptr;
    }
    return m_coverageTable[iTable].data();
}

// No idea when this is called? Just return a red coloured bitmap.
wxBitmap* DemoChartGL::GetThumbnail(int tnx, int tny, int cs) {
    bitMap.Create(tnx, tny, 32);
    wxMemoryDC dc;
    dc.SelectObject(bitMap);
    dc.Clear();
    dc.SetBackground(wxColour(255, 0, 0, 40));
    dc.SelectObject(wxNullBitmap);
    return &bitMap;
}

// The following are chart metadata. Observed when hovering over the "piano" key
// In reality these should be obtained when parsing a "proper" chart
// The fully distinguished name for the chart file
wxString DemoChartGL::GetFullPath() const {
    return m_chartPath;
}

ChartTypeEnumPI DemoChartGL::GetChartType() {
    return ChartTypeEnumPI::PI_CHART_TYPE_PLUGIN;
}

// This demo uses GeoJSON formatted charts, therefore these are vector charts
ChartFamilyEnumPI DemoChartGL::GetChartFamily() {
    return ChartFamilyEnumPI::PI_CHART_FAMILY_VECTOR;
}

// I don't believe the Natural Earth GEO JSON files use Mercator projection?
OcpnProjTypePI DemoChartGL::GetChartProjection() {
    return OcpnProjTypePI::PI_PROJECTION_MERCATOR;
}

wxString DemoChartGL::GetName() {
    return "Demo Chart";
}

wxString DemoChartGL::GetDescription() {
    return "Demo Chart Description";
}

wxString DemoChartGL::GetID() {
    return "Some Id";
}

wxString DemoChartGL::GetSE() {
    return "Demo Chart Authority";
}

wxString DemoChartGL::GetDepthUnits() {
    return "Metres";
}

wxString DemoChartGL::GetSoundingsDatum() {
    return "LAT";
}

wxString DemoChartGL::GetDatumString() {
    return "WGS84";
}

wxString DemoChartGL::GetExtraInfo() {
    return "Extra Info: Not to be used for navigation";
}

wxString DemoChartGL::GetPubDate() {
    return wxDateTime::Now().FormatISOCombined();
}

double DemoChartGL::GetChartErrorFactor() {
    return 0.0;
}

ChartDepthUnitTypePI DemoChartGL::GetDepthUnitId() {
    return ChartDepthUnitTypePI::PI_DEPTH_UNIT_METERS;
}

// Should get from the geojson file, the sample file was derived from a 1:50M chart
int DemoChartGL::GetNativeScale() {
    return 50000000;
}

double DemoChartGL::GetChartSkew() {
    return 0.0;
}

wxDateTime DemoChartGL::GetEditionDate(void) {
    return wxDateTime::Now();
}

// No coverage areas
int DemoChartGL::GetNoCOVREntries() {
    return 0;
}

int DemoChartGL::GetNoCOVRTablePoints(int iTable) {
    return 0;
}

int DemoChartGL::GetNoCOVRTablenPoints(int iTable) {
    return 0;
}

float* DemoChartGL::GetNoCOVRTableHead(int iTable) {
    return nullptr;
}

// Rendering routines
// If the plugin had a long running operation, return when appropriate
bool DemoChartGL::IsReadyToRender() {
    return true;
}

// Generate a vector of pairs of floats for all of the coverage areas (i.e polygons)
// Refer to GetCOVREntries etc.
// Appears to be used by OpenCPN to populate the chart database for vector charts?
void DemoChartGL::BuildCoverageFromFeatures() {

    m_coverageTable.clear();

    for (const auto& feature : m_chartFeatures) {
        if (feature.geometryType == "Polygon" || feature.geometryType == "MultiPolygon") {
            for (const auto& polygon : feature.geoPoints) {
                std::vector<float> coveragePoints;
                coveragePoints.reserve(polygon.size() * 2);
                // Fucking GeoJSON stores points in the wrong order
                // longitude, latitude, so swap them around....
                for (const auto& point : polygon) {
                    coveragePoints.push_back((float)point.y);
                    coveragePoints.push_back((float)point.x);
                }
                m_coverageTable.push_back(std::move(coveragePoints));
            }
        }
    }
}

// Render the chart when using OpenGL
// Requires INSTALLS_PLUGIN_CHART_GL
int DemoChartGL::RenderRegionViewOnGL(const wxGLContext& glc, const PlugIn_ViewPort& viewPort,
    const wxRegion& Region, bool b_use_stencil) {

    auto start = std::chrono::high_resolution_clock::now();

    // Lambda function to set the OpenGL colour depending on the layer
    auto setColour = [](wxString layer) {
        if (layer == "land") {
            glColor4f(0.30f, 0.65f, 0.20f, 0.5f); // Green (transparent)
        }
        else if (layer == "river") {
            glColor4f(0.15f, 0.45f, 0.90f, 1.0f); // Medium blue
        }
        else if (layer == "lake") {
            glColor4f(0.10f, 0.30f, 0.75f, 1.0f); // Darker blue
        }
        else if (layer == "coastline") {
            glColor4f(0.95f, 0.90f, 0.65f, 1.0f); // Sandy beige
        }
        else if (layer == "landedge") {
            glColor4f(0.30f, 0.65f, 0.20f, 1.0f); // Green (opaque)
        }
        else {
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Default: white
        }
    };

    // If OpenCPN is performing the clipping, we can just use wxRegion and OpenGL to  
    // automagically clip, rather than perform the clipping ourselves.
    if (b_use_stencil) {

        glClear(GL_STENCIL_BUFFER_BIT);

        glEnable(GL_STENCIL_TEST);

        // Every fragment that passes writes a "1" into the stencil.
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

        // Set the colour and depth mask, unused in this demo
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);

        wxRegionIterator it(Region);

        while (it) {
            int x = it.GetX();
            int y = it.GetY();
            int w = it.GetW();
            int h = it.GetH();

            glBegin(GL_QUADS);
            glVertex2i(x, y);
            glVertex2i(x + w, y);
            glVertex2i(x + w, y + h);
            glVertex2i(x, y + h);
            glEnd();

            ++it;
        }

        // Reset the colour & depth masks (unneccessary as they weren't used)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);

        // Only enable drawing where the stencil mask applies
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }

    // Clear the background and set it to a horrible mauve
    glClearColor(206.f / 255.f, 206.f / 255.f, 250.f / 255.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Following matrix operations apply to the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Create an orthographic projection (as compared to for example a perspective projection)
    // Note that the y-axis increases downwards, meaning the projection matches screen co-ordinates
    glOrtho(0.0, viewPort.pix_width, viewPort.pix_height, 0.0, -1.0, 1.0);

    // Following matrix operations apply to the model view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Enable "blending" ie. transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create the GLU Tesselator and register the callback functions
    GLUtesselator* tess = gluNewTess();
    gluTessCallback(tess, GLU_TESS_BEGIN, (GLvoid(CALLBACK*)()) tessBegin);
    gluTessCallback(tess, GLU_TESS_END, (GLvoid(CALLBACK*)()) tessEnd);
    gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid(CALLBACK*)()) tessVertex);
    gluTessCallback(tess, GLU_TESS_ERROR, (GLvoid(CALLBACK*)()) tessError);
    gluTessCallback(tess, GLU_TESS_COMBINE, (void (CALLBACK*)())tessCombine);

    // Set the tesselator winding rule for filled areas
    gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);

    // Cull the Quadtree
    std::vector<FeatureReference> culledFeatures;
    GeoBounds b;
    
    b.min.x = std::min(viewPort.lon_min, viewPort.lon_max);
    b.max.x = std::max(viewPort.lon_min, viewPort.lon_max);
    b.min.y = std::min(viewPort.lat_min, viewPort.lat_max);
    b.max.y = std::max(viewPort.lat_min, viewPort.lat_max);

    chartCache->Query(b, culledFeatures);

    wxLogDebug("Culled Features %d", culledFeatures.size());

    // Lookup each culled chart feature and render
    for (const auto& culledFeature : culledFeatures) {

        GeoFeature chartFeature = m_chartFeatures.at(culledFeature.index);

        setColour(chartFeature.layer);

        if (chartFeature.geometryType == "Polygon" || chartFeature.geometryType == "MultiPolygon") {
            for (const auto& polygon : chartFeature.geoPoints) {

                // Build screen-space vertices — GLU needs double[3]
                std::vector<Vert3> vertices;
                vertices.reserve(polygon.size());
                for (size_t i = 0; i < polygon.size(); ++i) {
                    wxPoint p;
                    GetCanvasPixLL((PlugIn_ViewPort*)&viewPort, &p, polygon[i].y, polygon[i].x);
                    Vert3 threeDVertex;
                    threeDVertex.x = (double)p.x;
                    threeDVertex.y = (double)p.y;
                    threeDVertex.z = 0.0;
                    vertices.push_back(threeDVertex);
                }

                // Enable colour blending
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                // Render the polygon
                gluTessBeginPolygon(tess, nullptr);
                gluTessBeginContour(tess);

                for (auto& vertex : vertices) {
                    gluTessVertex(tess, &vertex.x, &vertex.x); // Note to self, passing vertex.x as user data??
                }

                gluTessEndContour(tess);
                gluTessEndPolygon(tess);

                // Clear the memory allocated for combined vertices (if any)
                for (GLdouble* v : temporaryVertices) {
                    delete[] v;
                }
                temporaryVertices.clear();

                // Render the outline of the polygon - Use a different hard coded colour
                setColour("landedge");
              
                // Note GL_LINE_LOOP means the start & end vertices are to be joined together
                glBegin(GL_LINE_LOOP);
                for (const auto& v : vertices) {
                    glVertex2d(v.x, v.y);
                }
                glEnd();
            }
        }
        // Render lines such as rivers etc.
        else if (chartFeature.geometryType == "LineString" || chartFeature.geometryType == "MultiLineString") {
            
            for (const auto& line : chartFeature.geoPoints) {
                // Note GL_LINE_STRIP means the start & end vertices are not joined together
                glBegin(GL_LINE_STRIP);
                for (const auto& rawPt : line) {
                    wxPoint p;
                    GetCanvasPixLL((PlugIn_ViewPort*)&viewPort, &p, rawPt.y, rawPt.x);
                    glVertex2f(p.x, p.y);
                }
                glEnd();
            }
        }
    }
    
    // Clean up the GLU Tesselator callbacks
    gluDeleteTess(tess);

    if (b_use_stencil) {
        // Turn off the stencil mask
        glDisable(GL_STENCIL_TEST);
    }

    auto end = std::chrono::high_resolution_clock::now();
    wxLogDebug("Render Duration: %d", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    // Unsure what the return value should be....
    return 1;
}

