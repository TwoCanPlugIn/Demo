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


#ifndef DEMO_PLUGIN_H
#define DEMO_PLUGIN_H

// Pre compiled headers 
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif

// wxWidgets
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/fileconf.h>

// Defines version numbers, names etc. for this plugin
// This is automagically constructed via config.h.in from CMakeLists.txt
#include "config.h"

// OpenCPN Plugin header
#include "ocpn_plugin.h"

// Bundled OpenCPN NMEA 0183 libraries
#include "nmea0183.h"

// Bundled OpenCPN NMEA 2000 libraries
#include "N2KParser.h"

// Bundled OpenCPN JSON libraries
#include "wx/json_defs.h"
#include "wx/jsonreader.h"
#include "wx/jsonval.h"
#include "wx/jsonwriter.h"

class DemoPlugin : public opencpn_plugin_120, public wxEvtHandler {
public:
	// Constructor
	DemoPlugin(void* ppimgr);

	// Destructor
	~DemoPlugin(void);

	// Overridden OpenCPN plugin methods
	int Init(void) override;
	bool DeInit(void) override;
	int GetAPIVersionMajor() override;
	int GetAPIVersionMinor() override;
	int GetPlugInVersionMajor() override;
	int GetPlugInVersionMinor() override;
	int GetPlugInVersionPatch() override;
	wxString GetCommonName() override;
	wxString GetShortDescription() override;
	wxString GetLongDescription() override;
	wxBitmap* GetPlugInBitmap() override;
	void SetDefaults(void) override;
	void SetupToolboxPanel(int page_sel, wxNotebook* pnotebook) override;
	void OnCloseToolboxPanel(int page_sel, int ok_apply_cancel) override;
	void OnSetupOptions(void) override;
	void ShowPreferencesDialog(wxWindow* parent) override;
	void OnContextMenuItemCallback(int id) override;
	void OnContextMenuItemCallbackExt(int id, std::string obj_ident, std::string obj_type, double lat, double lon) override;
	void OnToolbarToolCallback(int id) override;
	void SetPositionFixEx(PlugIn_Position_Fix_Ex& pfix) override;
	void SetNMEASentence(wxString& sentence) override;
	void LateInit(void) override;
	void SetPluginMessage(wxString& message_id, wxString& message_body) override;

private:
	void LoadSettings();
	void SaveSettings();

	void GetAllWaypoints();
	void GetAllRoutes();
	void CreateWaypoint();

	// Context Menu Id's
	int demoContextMenuId;
	int demoAISContextMenuId;
	
	// Toolbar button Id & state
	int demoToolbarId;
	bool isToolbarActive;

	// Current position and heading
	double currentLatitude;
	double currentLongitude;
	double trueHeading;

	// Wind angle and speed
	double apparentWindAngle;
	double apparentWindSpeed;
	double trueWindAngle;
	double trueWindSpeed;
	double trueWindDirection;

	// Speed Through Water
	double boatSpeed;

	// Calculate True Wind
	void CalculateTrueWind(void);

	// Generate NMEA 0183 MWV Sentence for True Wind Angle
	wxString FormatTrueWindSentence(void);

	// Generate NMEA 2000 PGN 130306 message for True Wind Angle
	std::vector<uint8_t> FormatTrueWindMessage(void);

	// Function to parse NMEA0183 MWV sentences
	void ParseWind(NMEA0183* nmea0183Sentence);

	// Helper function to find a required connection
	std::string FindOutboundConnection(const std::string& connectionType);

	// Transmit NMEA 0183 data using observer/listener model
	// An interface for a NMEA 0183 connection
	DriverHandle nmea0183Driver;
	void SendNMEA0183(const std::string& driverHandle, const std::string& sentence);

	// Transmit NMEA 2000 data using observer/listener model
	// An interface for a NMEA 2000 connection
	DriverHandle nmea2000Driver;
	void SendNMEA2000(const std::string& driverHandle, const unsigned char& destination, 
		const unsigned char& priority,	const unsigned int pgn,	std::vector<uint8_t>& payload);

	// Transmit Plugin Messages using observer/listener model
	// The internal interface for Plugin Messsaging
	DriverHandle messagingDriver;
	void SendJSONMessage(const std::string& driverHandle, const std::string& message);

	// Generate JSON message for True Wind Angle
	wxString FormatTrueWindJSON(void);

	// Parse SignalK update messages
	wxString selfURN;
	void HandleSKUpdate(wxJSONValue& update);

	// New Observer Listener model handlers
	
	// Used instead of SetPositionFix callback API
	void HandleNavData(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_nav;

	// NMEA 0183 VHW Boat speed
	void HandleVHW(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_vhw;

	// NMEA 2000 Boat speed
	void HandleN2K_128259(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_128259;

	// NMEA 2000 Wind Speed
	void HandleN2K_130306(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_130306;

	// OpenCPN Plugin Messaging
	void HandleMsg_RouteActivated(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_route;

	// OpenCPN SignalK
	void HandleSignalK(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_signalk;
};

#endif 

