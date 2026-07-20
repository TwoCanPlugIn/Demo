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
// Project: Active Captain Plugin
// Description: A variation of the Demo plugin, fetching Garmin Active Captain  Points of Interest
// Owner: twocanplugin@hotmail.com
// Date: 10/01/2026
// Version History: 
// 1.0 Initial Release
// 1.0.1, 28/04/2026 - Updated for Active Captain POC
// To contemplate for a future release (if any)
// Retrieve more POI's and store in a quadtree to support pan & zoom
// Use the Active Captain SDK to store a local copy of the Active Captain database and allow users to comment or review

#include "demo_plugin.h"

// The class factories, used to create and destroy instances of the PlugIn
extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr) {
	return new DemoPlugin(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) {
	delete p;
}

// Constructor
// As the plugin uses newer functions, it requires a plugin API version of at least 1.20
DemoPlugin::DemoPlugin(void* ppimgr) : opencpn_plugin_120(ppimgr), wxEvtHandler() {
	
	// All the bitmaps used by the plugin, both for itself and for rendering POI's on the chart
	wxString pluginFolder = GetPluginDataDir(PKG_NAME) + wxFileName::GetPathSeparator() + "data" + wxFileName::GetPathSeparator();
	
	// Garmin Active Captain Logo used for Plugin Manager and Settings dialogs
	pluginBitmap.LoadFile(pluginFolder + "garmin.bmp", wxBITMAP_TYPE_BMP);
	wxBitmap::Rescale(pluginBitmap, wxSize(32, 32));

	// Similar to above but with black border to match toolbar panel background
	toolbarBitmap.LoadFile(pluginFolder + "garmin-toolbar.bmp", wxBITMAP_TYPE_BMP);
	wxBitmap::Rescale(toolbarBitmap, wxSize(32, 32));

	// Initialize the Garmin POI bitmaps (These were converted from the official Garmin POI icons)
	bitmapMarina.LoadFile(pluginFolder + "marina.bmp", wxBITMAP_TYPE_BMP);
	bitmapAnchorage.LoadFile(pluginFolder + "anchorage.bmp", wxBITMAP_TYPE_BMP);
	bitmapHazard.LoadFile(pluginFolder + "hazard.bmp", wxBITMAP_TYPE_BMP);
	bitmapBusiness.LoadFile(pluginFolder + "shop.bmp", wxBITMAP_TYPE_BMP);
	bitmapBoatRamp.LoadFile(pluginFolder + "boat_ramp.bmp", wxBITMAP_TYPE_BMP);
	bitmapBridge.LoadFile(pluginFolder + "bridge.bmp", wxBITMAP_TYPE_BMP);
	bitmapDam.LoadFile(pluginFolder + "dam.bmp", wxBITMAP_TYPE_BMP);
	bitmapFerry.LoadFile(pluginFolder + "ferry.bmp", wxBITMAP_TYPE_BMP);
	bitmapInlet.LoadFile(pluginFolder + "inlet.bmp", wxBITMAP_TYPE_BMP);
	bitmapLock.LoadFile(pluginFolder + "lock.bmp", wxBITMAP_TYPE_BMP);
	bitmapDefault.LoadFile(pluginFolder + "garmin.bmp", wxBITMAP_TYPE_BMP);
	
	// BUG BUG - Could either have scaled bitmaps for different viewport scales, or rescale on the fly
	wxBitmap::Rescale(bitmapMarina, wxSize(32, 32));
	wxBitmap::Rescale(bitmapAnchorage, wxSize(32, 32));
	wxBitmap::Rescale(bitmapHazard, wxSize(32, 32));
	wxBitmap::Rescale(bitmapBusiness, wxSize(32, 32));
	wxBitmap::Rescale(bitmapBoatRamp, wxSize(32, 32));
	wxBitmap::Rescale(bitmapBridge, wxSize(32, 32));
	wxBitmap::Rescale(bitmapDam, wxSize(32, 32));
	wxBitmap::Rescale(bitmapFerry, wxSize(32, 32));
	wxBitmap::Rescale(bitmapInlet, wxSize(32, 32));
	wxBitmap::Rescale(bitmapLock, wxSize(32, 32));
	wxBitmap::Rescale(bitmapDefault, wxSize(32, 32));
}

// Destructor
DemoPlugin::~DemoPlugin(void) {
}

// Perform plugin initialization here. At this point most of OpenCPN has been initialised and most of the plugin API's can be invoked
int DemoPlugin::Init(void) {
	
	// Load the previously saved settings
	LoadSettings();

	// The context menu item allows the user to retrieve information about the selected point of interest
	auto garminContextMenu = new wxMenuItem(NULL, k_GarminContextMenu, "Garmin (POI)", "Garmin Active Captain Plugin", wxITEM_NORMAL, NULL);
	garminContextMenuId = AddCanvasContextMenuItem(garminContextMenu, this);

	// The toolbar button retrieves the POI's for the current viewport
	// Ideally the POI's should be retrieved automatically as the viewport is panned or zoom level changes
	
	// Using this instead of InsertPlugiInToolSVG as don't have Garmin icons in SVG format.
	garminToolBarId = InsertPlugInTool("Garmin Active Captain", &toolbarBitmap, &toolbarBitmap,wxITEM_CHECK, 
		"Garmin Active Captain", "Fetch Garmin Active Captain Points Of Interest", NULL, -1, 0, this);

	// A flag used to indicate the toggled/untoggled state of the toolbar button
	isToolbarActive = false;

	// Notify OpenCPN what callbacks the plugin registers to receive
	return (WANTS_CONFIG | INSTALLS_TOOLBOX_PAGE | WANTS_PREFERENCES | INSTALLS_TOOLBAR_TOOL | 
		WANTS_OVERLAY_CALLBACK | WANTS_OPENGL_OVERLAY_CALLBACK | WANTS_ONPAINT_VIEWPORT |
		WANTS_CURSOR_LATLON | WANTS_MOUSE_EVENTS);
}

// OpenCPN is either closing down, or the plugin has been disabled from the Preferences Dialog
bool DemoPlugin::DeInit(void) {

	// Note, OpenCPN doesn't actually care about the return value
	return true; 
}

// OpenCPN Plugin "housekeeping" methods. All plugins MUST implement these

// Indicate what version of the OpenCPN Plugin API is supported
// Note if using the TestPlugin template, replace these with OCPN_API_VERSION_MAJOR 
// and OCPN_API_VERSION_MINOR
int DemoPlugin::GetAPIVersionMajor() {
	return atoi(API_VERSION);
}

int DemoPlugin::GetAPIVersionMinor() {
	std::string v(API_VERSION);
	size_t dotpos = v.find('.');
	return atoi(v.substr(dotpos + 1).c_str());
}

// The plugin version numbers. 
int DemoPlugin::GetPlugInVersionMajor() {
	return PLUGIN_VERSION_MAJOR;
}

int DemoPlugin::GetPlugInVersionMinor() {
	return PLUGIN_VERSION_MINOR;
}

// BUG BUG Need to add patch version to config.h.in
int DemoPlugin::GetPlugInVersionPatch() {
	return 0;
}

// Return descriptions for the Plugin
// Note if using TestPlugin template use PLUGIN_COMMON_NAME
wxString DemoPlugin::GetCommonName() {
	return PLUGIN_API_NAME;
}

// Note if using TestPlugin template use PLUGIN_SHORT_DESCRIPTION
wxString DemoPlugin::GetShortDescription() {
	return PKG_SUMMARY;
}

// Note if using TestPlugin template use PLUGIN_LONG_DESCRIPTION
wxString DemoPlugin::GetLongDescription() {
	return PKG_DESCRIPTION;
}

// The plugin's bitmap was initialised earlier
wxBitmap* DemoPlugin::GetPlugInBitmap() {
	return &pluginBitmap;
}

// End of mandatory "housekeeping" methods

// When the plugin is enabled, this API provides the opportunity to configure initial settings
void DemoPlugin::SetDefaults(void) {
	auto installationWizard = std::make_unique<DemoWizard>(GetOCPNCanvasWindow());
	if (installationWizard->RunWizard(installationWizard->m_pages.at(0))) {
		SaveSettings();
	}
}

// Invoked when the OpenCPN Toolbox OK, Apply or Cancel buttons are pressed
void DemoPlugin::OnCloseToolboxPanel(int page_sel, int ok_apply_cancel) {
	// BUG BUG Why didn't they use standard enums like wxID_OK ??	
	if ((ok_apply_cancel == 0) || (ok_apply_cancel == 4)) {
		SaveSettings();
	}
}

// Invoked at Startup and displayed when the OpenCPN Toolbox is displayed
// Requires INSTALLS_TOOLBOX_PAGE
void DemoPlugin::OnSetupOptions(void) {
	// Add our toolbox to the "User Interface" tab
	auto toolBoxWindow = AddOptionsPage(OptionsParentPI::PI_OPTIONS_PARENT_UI, _("Active Captain"));
	auto toolboxSizer = new wxBoxSizer(wxVERTICAL);
	toolBoxWindow->SetSizer(toolboxSizer);
	// Create our toolbox panel and add it to the toolbox via the sizer
	auto demoToolbox =  new DemoToolbox(toolBoxWindow);
	toolboxSizer->Add(demoToolbox, 1, wxALL | wxEXPAND);
}

// Invoked from the plugin's preferences option, enabling the user to modify the plugin's settings.
// Requires WANTS_PREFERENCES
void DemoPlugin::ShowPreferencesDialog(wxWindow* parent) {
	auto demoSettings = std::make_unique<DemoSettings>(parent, wxID_ANY, _("Active Captain Preferences"));
	if (wxID_OK == demoSettings->ShowModal()) {
		SaveSettings();
	}
}

// Invoked when the plugin's context menu items are selected
void DemoPlugin::OnContextMenuItemCallback(int id) {
	// SetCursorLatLon should have set the POI id if the cursor was on a PI, otherwise it is an empty string
	if (id == garminContextMenuId) {
		RequestGarminPointOfInterest(pointOfInterestId);
	}
}

// Invoked when the plugin's toolbar button is presssed
void DemoPlugin::OnToolbarToolCallback(int id) {
	if (id == garminToolBarId) {
		// Retrieve all points of interest for the current viewport
		isToolbarActive = !isToolbarActive;
		SetToolbarItemState(id, isToolbarActive);
		RequestGarminPointOfInterest();
		if (pointsOfInterest.size() > 0) {
			RequestRefresh(GetOCPNCanvasWindow());
		}
		isToolbarActive = !isToolbarActive;
		SetToolbarItemState(id, isToolbarActive);
	}
}

// This callback provides the cursorposition. Use it to determine if over a point of interest, 
// and enable/disable the context menu
void DemoPlugin::SetCursorLatLon(double lat, double lon) {
	if (IsUnderCursor(lat, lon, &pointOfInterestId)) {
		SetCanvasContextMenuItemGrey(garminContextMenuId, false);
	}
	else {
		SetCanvasContextMenuItemGrey(garminContextMenuId, true);
	}
}

// When the user double clicks on the POI, retrieve the detailed information for the POI
// BUG BUG, Seems to be broken on non OpenGL ??
bool DemoPlugin::MouseEventHook(wxMouseEvent& event) {
	
	// Only perform these actions if we have an Internet connection
	if (!OCPN_isOnline()) {
		return false;
	}
	double lat, lon;

	// Handle a double click event to retrieve the POI information
	if (event.LeftDClick()) {

		// Convert Pixel Co-ordinates to latitude and longitude
		GetCanvasLLPix(&viewPort, event.GetPosition(), &lat, &lon);

		// Iterate over the points of interest and see if any  were the double click target
		if (IsUnderCursor(lat, lon, &pointOfInterestId)) {
			// Request the detailed information for the POI
			RequestGarminPointOfInterest(pointOfInterestId);
			return true;
		}
	}
	// Returning false allows the event to be forwarded to OpenCPN (eg. chart object properties)
	// whereas true means we handled the event and the event is not propogated
	return false;
}

// Persist the viewport as it is used to determine the bounding box for retrieving Active Captain POI's
void DemoPlugin::SetCurrentViewPort(PlugIn_ViewPort& vp) {

	viewPort = vp;
}

// Determine if the cursor is over a point of interest
// BUG BUG Probably has a crossing International Date Line bug
bool DemoPlugin::IsUnderCursor(const double lat, const double lon, wxString* id) {

	*id = wxEmptyString;

	// Calculate the hit test dimensions based on the bitmap and viewport sizes
	const auto bitmapSize = bitmapMarina.GetSize();
	const double bitmapHeight =	bitmapSize.GetHeight() * (viewPort.lat_max - viewPort.lat_min) / viewPort.pix_height;
	const double bitmapWidth = bitmapSize.GetWidth() *	(viewPort.lon_max - viewPort.lon_min) /	viewPort.pix_width;
	
	// Co-ordinates of the hit test.
	const double top = lat + bitmapHeight;
	const double bottom = lat - bitmapHeight;
	const double left = lon - bitmapWidth;
	const double right = lon + bitmapWidth;

	// If we had a large number of POI's either a quadtree or hash table would improve performance
	for (auto& it : pointsOfInterest) {
		if ((it.latitude >= bottom) && (it.latitude <= top) && (it.longitude >= left) && (it.longitude <= right)) {
				*id = it.id;
				return true;
		}
	}
	return false;
}

// Drawing on the Canvas when OpenGL is not being used
// Note requires WANTS_OVERLAY_CALLBACK and that OpenCPN is NOT using 
// OpenGL (hardware graphics acceleration)
bool DemoPlugin::RenderOverlayMultiCanvas(wxDC& dc, PlugIn_ViewPort* vp,
	int canvasIndex, int priority) {

	// Only draw in Legacy Mode, other modes are OVERLAY_OVER_SHIPS, OVERLAY_OVER_UI etc.
	if (priority != OVERLAY_LEGACY) {
		wxLogDebug("Active Captain Plugin, Render error, OpenCPN not in legacy mode, %d", priority);
		return false;
	}

	if (!dc.IsOk()) {
		wxLogDebug("Active Captain Plugin, Render error, Canvas DC not OK");
		return false;
	}

	// Only draw on a valid canvas
	if ((canvasIndex != 0) && (canvasIndex != 1)) {
		return false;
	}

	// Pre compute the chart extents, only draw the POI's that are in the viewport.

	const double south = std::min(vp->lat_min, vp->lat_max);
	const double north = std::max(vp->lat_min, vp->lat_max);
	const double west = std::min(vp->lon_min, vp->lon_max);
	const double east = std::max(vp->lon_min, vp->lon_max);

	// At present only support one screen
	if (canvasIndex == 0) {

		// BUG BUG, If the user has panned/zoomed, the vector should be culled otherwise we're checking values that 
		// will never be rendered. Could always contemplate a quadtree, especially from a performance perspective
		for (auto& it : pointsOfInterest) {
			wxPoint wxP;
			GetCanvasPixLL(vp, &wxP, it.latitude, it.longitude);

			if (it.latitude >= south &&
				it.latitude <= north &&
				it.longitude >= west &&
				it.longitude <= east) {

				switch (it.type) {
				case Category::Marina:
					dc.DrawBitmap(bitmapMarina, wxP.x, wxP.y, true);
					break;
				case Category::Anchorage:
					dc.DrawBitmap(bitmapAnchorage, wxP.x, wxP.y, true);
					break;
				case Category::Business:
					dc.DrawBitmap(bitmapBusiness, wxP.x, wxP.y, true);
					break;
				case Category::BoatRamp:
					dc.DrawBitmap(bitmapBoatRamp, wxP.x, wxP.y, true);
					break;
				case Category::Bridge:
					dc.DrawBitmap(bitmapBridge, wxP.x, wxP.y, true);
					break;
				case Category::Dam:
					dc.DrawBitmap(bitmapDam, wxP.x, wxP.y, true);
					break;
				case Category::Ferry:
					dc.DrawBitmap(bitmapFerry, wxP.x, wxP.y, true);
					break;
				case Category::Hazard:
					dc.DrawBitmap(bitmapHazard, wxP.x, wxP.y, true);
					break;
				case Category::Inlet:
					dc.DrawBitmap(bitmapInlet, wxP.x, wxP.y, true);
					break;
				case Category::Lock:
					dc.DrawBitmap(bitmapLock, wxP.x, wxP.y, true);
					break;
				default: // Unaware of any POI's that have a different type
					dc.DrawBitmap(bitmapDefault, wxP.x, wxP.y, true);
					break;
				}
			}
		}
	}
	return true;
}

// Drawing on the canvas using OpenGL (hardware accelerated graphics)
// Note requires WANTS_OPENGL_OVERLAY_CALLBACK
bool DemoPlugin::RenderGLOverlayMultiCanvas(wxGLContext* pcontext, PlugIn_ViewPort* vp,
	int canvasIndex, int priority) {

	// Draw "over" the chart, other modes are OVERLAY_LEGACY, OVERLAY_OVER_SHIPS, OVERLAY_OVER_UI etc.
	if (priority != OVERLAY_OVER_EMBOSS) {
		wxLogDebug("Active Captain Plugin, OpenGL Render error, OpenCPN not in emboss mode, %d", priority);
		return false;
	}

	if (!pcontext->IsOK()) {
		wxLogDebug("Active Captain Plugin, OpenGL Render error, Canvas DC not OK");
		return false;
	}

	// At present only support one screen
	if (canvasIndex == 0) {

		const double south = std::min(vp->lat_min, vp->lat_max);
		const double north = std::max(vp->lat_min, vp->lat_max);
		const double west = std::min(vp->lon_min, vp->lon_max);
		const double east = std::max(vp->lon_min, vp->lon_max);

		// The pluginDC helper class that abstracts wxGLCanvas methods It is  "universal" in that
		// it support both OpenGL (wxGLCanvas) and non OpenGL (wxDC).
		std::unique_ptr<piDC> pluginDC = std::make_unique<piDC>(pcontext);

		for (auto& it : pointsOfInterest) {
			wxPoint wxP;
			GetCanvasPixLL(vp, &wxP, it.latitude, it.longitude);

			if (it.latitude >= south &&
				it.latitude <= north &&
				it.longitude >= west &&
				it.longitude <= east) {

				switch (it.type) {
				case Category::Marina:
					pluginDC->DrawBitmap(bitmapMarina, wxP.x, wxP.y, true);
					break;
				case Category::Anchorage:
					pluginDC->DrawBitmap(bitmapAnchorage, wxP.x, wxP.y, true);
					break;
				case Category::Business:
					pluginDC->DrawBitmap(bitmapBusiness, wxP.x, wxP.y, true);
					break;
				case Category::BoatRamp:
					pluginDC->DrawBitmap(bitmapBoatRamp, wxP.x, wxP.y, true);
					break;
				case Category::Bridge:
					pluginDC->DrawBitmap(bitmapBridge, wxP.x, wxP.y, true);
					break;
				case Category::Dam:
					pluginDC->DrawBitmap(bitmapDam, wxP.x, wxP.y, true);
					break;
				case Category::Ferry:
					pluginDC->DrawBitmap(bitmapFerry, wxP.x, wxP.y, true);
					break;
				case Category::Hazard:
					pluginDC->DrawBitmap(bitmapHazard, wxP.x, wxP.y, true);
					break;
				case Category::Inlet:
					pluginDC->DrawBitmap(bitmapInlet, wxP.x, wxP.y, true);
					break;
				case Category::Lock:
					pluginDC->DrawBitmap(bitmapLock, wxP.x, wxP.y, true);
					break;
				default:
					pluginDC->DrawBitmap(bitmapDefault, wxP.x, wxP.y, true);
					break;
				}
			}
		}
	}
	return true;
}

// Helper functions to map enums to an int & vice-versa for Point of Interest types (marina, anchorage etc.)
Category DemoPlugin::StringToCategory(const wxString& s) {
	for (const auto& c : categories) {
		if (s == c.name)
			return c.category;
	}

	return Category::Default;
}

wxString DemoPlugin::CategoryToString(const Category& c) {
	for (const auto& entry : categories) {
		if (entry.category == c)
			return entry.name;
	}
	return "Unknown";
}


// Retrieve the list of selected POI's for the current viewport
bool DemoPlugin::RequestGarminPointOfInterest(void) {

	if (!OCPN_isOnline()) {
		return false;
	}

	wxString url = "https://activecaptain.garmin.com/community/api/v1/points-of-interest/bbox";
	
	wxJSONValue bbox;
	wxJSONWriter writer;
	wxString jsonBody;

	// JSON text body
	bbox["north"] = viewPort.lat_max;
	bbox["south"] = viewPort.lat_min;
	bbox["east"] = viewPort.lon_max;
	bbox["west"] = viewPort.lon_min;
	bbox["zoomLevel"] = 17;
	
	wxString poiTypes;

	auto appendItem = [&poiTypes](const wxString& item)
		{
			if (!poiTypes.empty()) {
				poiTypes += ", ";
			}
			poiTypes += item;
		};

	// Select which Points of Interest to fetch
	if (g_showMarina) {
		appendItem(wxString("Marina"));
	}
	if (g_showAnchorage) {
		appendItem(wxString("Anchorage"));
	}
	if (g_showHazard) {
		appendItem(wxString("Hazard"));
	}
	if (g_showBusiness) {
		appendItem(wxString("Business"));
	}
	if (g_showBoatRamp) {
		appendItem(wxString("BoatRamp"));
	}
	if (g_showBridge) {
		appendItem(wxString("Bridge"));
	}
	if (g_showDam) {
		appendItem(wxString("Dam"));
	}
	if (g_showFerry) {
		appendItem(wxString("Ferry"));
	}
	if (g_showInlet) {
		appendItem(wxString("Inlet"));
	}
	if (g_showDam) {
		appendItem(wxString("Lock"));
	}

	bbox["poiTypes"] = poiTypes;
	
	// Construct the JSON text
	writer.Write(bbox, jsonBody);

	// BBUG BUG, This is an asynch call?
	bool result = false;
	// Create the request object
	wxWebRequest request = wxWebSession::GetDefault().CreateRequest(this, url);

	// Specify the Post method
	request.SetMethod("POST");
	// Headers
	request.SetHeader("User-Agent", "OpenCPN Active Captain Plugin");
	request.SetHeader("Content-Type", "application/json");
	request.SetHeader("Accept", "application/json");

	// Finally the data itself.
	request.SetData(jsonBody, "application/json");

	if (!request.IsOk()) {
		wxLogError("Active Captain Plugin, Unexepected Web Request Error");
		return false;
	}

	// The web request event handler (just a lambda function)
	Bind(wxEVT_WEBREQUEST_STATE, [this, &result](wxWebRequestEvent& evt) {
		switch (evt.GetState()) {
		case wxWebRequest::State_Completed:
			SavePointOfInterest(evt.GetResponse().AsString());
			result = true;
			break;

		case wxWebRequest::State_Failed:
			wxMessageBox(wxString::Format("Failed to send Request to Active Captain %s",evt.GetErrorDescription()), 
				"Active Captain Plugin");
			result = false;
			break;
			// Possibly handle	wxWebRequest::State_Unauthorized etc.
		}
		});

	// Start the request
	request.Start();
	
	return result;
}

// Request summary information for the selected point of interest
bool DemoPlugin::RequestGarminPointOfInterest(const wxString& id) {

	if (id.IsEmpty()) {
		return false;
	}

	wxString url = wxString::Format("https://activecaptain.garmin.com/community/api/v1/points-of-interest/%s/summary", id);

	bool result = false;
	// Create the request object
	wxWebRequest request = wxWebSession::GetDefault().CreateRequest(this, url);

	// Specify the Get method
	request.SetMethod("GET");
	// Headers
	request.SetHeader("User-Agent", "OpenCPN Active Captain Plugin");
	request.SetHeader("Content-Type", "application/json");
	request.SetHeader("Accept", "application/json");

	if (!request.IsOk()) {
		wxLogError("Active Captain Plugin, Unexepected Web Request Error");
		return false;
	}

	// The web request event handler (just a lambda function)
	Bind(wxEVT_WEBREQUEST_STATE, [this, &result](wxWebRequestEvent& evt) {
		switch (evt.GetState()) {
		case wxWebRequest::State_Completed:
			DisplayPointOfInterest(evt.GetResponse().AsString());
			result = true;
			break;

		case wxWebRequest::State_Failed:
			wxMessageBox(wxString::Format("Failed to send Request to Active Captain %s", evt.GetErrorDescription()),
				"Active Captain Plugin");
			result = false;
			break;
			// Possibly handle	other errors; wxWebRequest::State_Unauthorized etc.
		}
		});

	// Start the request
	request.Start();
	
	return result;
}

// Persist Active Captain Points of Interest
void DemoPlugin::SavePointOfInterest(const wxString& response) {

	wxJSONReader reader;
	wxJSONValue root;
	ActiveCaptainPOI pointOfInterest;

	if (reader.Parse(response, &root) > 0) {
		wxLogMessage("Active Captain Plugin, Json parser error(s) retrieving points of interest: %s");
		for (auto it : reader.GetErrors()) {
			wxLogMessage("Active Captain Plugin, Json parser error: %s", it);
		}
		return;
	}
	else {
		wxJSONValue pois = root["pointsOfInterest"];
		for (int i = 0; i < pois.Size(); i++) {
			wxJSONValue& poi = pois[i];
			pointOfInterest.id = poi["id"].AsString();
			pointOfInterest.name = poi["name"].AsString();
			pointOfInterest.type = StringToCategory(poi["poiType"].AsString());
			pointOfInterest.latitude = poi["mapLocation"]["latitude"].AsDouble();
			pointOfInterest.longitude = poi["mapLocation"]["longitude"].AsDouble();

			if (!IsDuplicate(pointOfInterest)) {
				pointsOfInterest.push_back(pointOfInterest);
			}
		}
	}
	// Just for the hell of it, log the number of POI's. May be helpful to determine if a Quadtree is required
	wxLogMessage("Active Captain Plugin, Number of POI's: %d", pointsOfInterest.size());
}

// Avoid adding duplicated objects
bool DemoPlugin::IsDuplicate(ActiveCaptainPOI& poi) {
	for (auto& it : pointsOfInterest) {
		if (it.id == poi.id) {
			return true;
		}
	}
	return false;
}

// Display Active Captain Point of Interest Details
void DemoPlugin::DisplayPointOfInterest(const wxString& response) {

	// Need to handle/display this kind of data. Dynamically add tabs to the dialog?

	/*
	* 
	* 
	
	{
	  "amenity": {
		"bar": "Unknown",
		"boatRamp": "Unknown",
		"cellReception": "Unknown",
		"courtesyCar": "Unknown",
		"dateLastModified": "2019-03-13T15:05:45.204",
		"id": 531966,
		"laundry": "Unknown",
		"lodging": "Unknown",
		"pets": "Unknown",
		"restaurant": "Unknown",
		"restroom": "Unknown",
		"shower": "Unknown",
		"transportation": "Unknown",
		"trash": "Unknown",
		"water": "Unknown",
		"wifi": "Unknown"
	  },
	  "business": {
		"cash": "Unknown",
		"check": "Unknown",
		"credit": "Unknown",
		"dateLastModified": "2019-03-13T15:05:45.719",
		"id": 531966,
		"public": "Unknown",
		"seasonal": "Unknown",
		"notes": []
	  },
	  "contact": {
		"id": 531966,
		"dateLastModified": "2019-03-13T15:05:45.141",
		"vhfChannel": "",
		"addressStreet": "88 BARWON HEADS ROAD",
		"addressCity": "BELMONT",
		"addressZip": "3216",
		"addressState": "VIC",
		"addressCountry": "Austraila",
		"website": "http://www.bargainboatbits.com.au",
		"afterHourContact": "",
		"phone": "",
		"email": ""
	  },
	  "fuel": {
		"currency": "UNK",
		"dateLastModified": "2019-03-13T15:05:45.188",
		"diesel": "Unknown",
		"ethanolFree": "Unknown",
		"gas": "Unknown",
		"id": 531966,
		"propane": "Unknown",
		"electric": "Unknown",
		"volumeUnits": "Gallon",
		"distanceUnit": "Meter"
	  },
	  "retail": {
		"dateLastModified": "2019-03-13T15:05:45.251",
		"fishingSupplies": "Unknown",
		"grocery": "Unknown",
		"hardware": "Unknown",
		"ice": "Unknown",
		"marineRetail": "Yes",
		"id": 531966
	  },
	  "services": {
		"boatBrokers": "Unknown",
		"bottomPainting": "Unknown",
		"canvasAndUpholstery": "Unknown",
		"carpentry": "Unknown",
		"charter": "Unknown",
		"dateLastModified": "2019-03-13T15:05:45.266",
		"electronics": "Unknown",
		"fiberglass": "Unknown",
		"haulOut": "Unknown",
		"id": 531966,
		"marineHvac": "Unknown",
		"mechanical": "Unknown",
		"paint": "Unknown",
		"plumbing": "Unknown",
		"propellerRepair": "Unknown",
		"pumpOut": "Unknown",
		"repair": "Unknown",
		"repairDieselEngines": "Unknown",
		"repairGasEngines": "Unknown",
		"rescueAndSalvage": "Unknown",
		"sailsAndRigging": "Unknown",
		"storage": "Unknown",
		"surveyors": "Unknown",
		"towing": "Unknown",
		"washAndWax": "Unknown",
		"waterTaxi": "Unknown",
		"welding": "Unknown"
	  },
	  "pointOfInterest": {
		"dateLastModified": "2019-04-13T16:00:01.521",
		"id": 531966,
		"mapLocation": {
		  "latitude": -38.1783539883777,
		  "longitude": 144.35342341661456
		},
		"name": "BARGAIN BOAT BITS",
		"poiType": "Business"
	  }
	}


	* 
	* 
	*/

	wxJSONReader reader;
	wxJSONValue root;
	ActiveCaptainPOI poi;
	
	if (reader.Parse(response, &root) > 0) {
		wxLogMessage("Active Captain Plugin, Json parser error(s) retrieving points of interest: %s");
		for (auto it : reader.GetErrors()) {
			wxLogMessage("Active Captain Plugin, Json parser error: %s", it);
		}
		return;
	}
	else {
		poi.id = root["pointOfInterest"]["id"].AsString();
		poi.name = root["pointOfInterest"]["name"].AsString();
		poi.type = StringToCategory(root["pointOfInterest"]["poiType"].AsString());
		poi.latitude = root["pointOfInterest"]["mapLocation"]["latitude"].AsDouble();
		poi.longitude = root["pointOfInterest"]["mapLocation"]["longitude"].AsDouble();
		if (root["pointOfInterest"].HasMember("notes") && root["pointOfInterest"]["notes"].IsArray() 
			&& root["pointOfInterest"]["notes"].Size() == 1) {
			poi.notes = root["pointOfInterest"]["notes"][0]["value"].AsString();
		}

		std::unique_ptr<DemoActiveCaptain> poiDialog = std::make_unique<DemoActiveCaptain>(poi, GetOCPNCanvasWindow(), wxID_ANY);
		poiDialog->ShowModal();
		
	}
}

// Plugin Settings determine what Points of Interest to retrieve
void DemoPlugin::LoadSettings() {
	wxFileConfig* configSettings = GetOCPNConfigObject();
	if (configSettings) {
		configSettings->SetPath("/PlugIns/ActiveCaptainPlugin");
		configSettings->Read("Marina", &g_showMarina, false);
		configSettings->Read("Anchorage", &g_showAnchorage, false);
		configSettings->Read("Hazard", &g_showHazard, false);
		configSettings->Read("Business", &g_showBusiness, false);
		configSettings->Read("BoatRamp", &g_showBoatRamp, false);
		configSettings->Read("Bridge", &g_showBridge, false);
		configSettings->Read("Dam", &g_showDam, false);
		configSettings->Read("Ferry", &g_showFerry, false);
		configSettings->Read("Inlet", &g_showInlet, false);
		configSettings->Read("Lock", &g_showLock, false);
	}
}


void DemoPlugin::SaveSettings() {
	wxFileConfig* configSettings = GetOCPNConfigObject();
	if (configSettings) {
		configSettings->SetPath("/PlugIns/ActiveCaptainPlugin");
		configSettings->Write("Marina", g_showMarina);
		configSettings->Write("Anchorage", g_showAnchorage);
		configSettings->Write("Hazard", g_showHazard);
		configSettings->Write("Business", g_showBusiness);
		configSettings->Write("BoatRamp", g_showBoatRamp);
		configSettings->Write("Bridge", g_showBridge);
		configSettings->Write("Dam", g_showDam);
		configSettings->Write("Ferry", g_showFerry);
		configSettings->Write("Inlet", g_showInlet);
		configSettings->Write("Lock", g_showLock);
	}
}