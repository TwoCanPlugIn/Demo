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
	void ShowPreferencesDialog(wxWindow* parent) override;
	void LateInit(void) override;

private:
	void LoadSettings();
	void SaveSettings();

	// Encode NMEA 0183 Sentences
	wxString GenerateMWV(double windAngle, double WindSpeed);

	wxString GenerateDPT(double depth, double offset);

	wxString GenerateGLL(double latitude, double longitude);

	wxString GenerateVHW(double speed);

	// Encode NMEA 2000 PGN's
	std::vector<uint8_t> GeneratePGN130306(double windSpeed, double windAngle);

	std::vector<uint8_t> GeneratePGN128259(double waterSpeed);

	std::vector<uint8_t> GeneratePGN128267(double depth, double offset);

	std::vector<uint8_t> GeneratePGN129025(double latitude, double longitude);

	// Transmit NMEA 0183 data using observer/listener model
	void SendNMEA0183(const std::string& driverHandle, const std::string& sentence);

	// Transmit NMEA 2000 data using observer/listener model
	void SendNMEA2000(const std::string& driverHandle, const unsigned char& destination, 
		const unsigned char& priority,	const unsigned int pgn,	std::vector<uint8_t>& payload);

	// New Observer Listener model handlers
	
	// NMEA 0183 VHW Boat speed
	void ParseVHW(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_vhw;

	// NMEA 0183 MWV Wind
	void ParseMWV(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_mwv;

	// NMEA 0183 DPT Depth
	void ParseDPT(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_dpt;

	// NMEA 0183 GLL Position
	void ParseGLL(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_gll;

	// NMEA 2000 Boat speed
	void HandleN2K_128259(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_128259;

	// NMEA 2000 Wind Speed
	void HandleN2K_130306(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_130306;

	// NMEA 2000 Depth
	void HandleN2K_128267(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_128267;

	// NMEA 2000 Position (Rapid Update)
	void HandleN2K_129025(ObservedEvt ev);
	std::shared_ptr<ObservableListener> listener_129025;

};

#endif 