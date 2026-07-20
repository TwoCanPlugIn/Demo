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


// Project: Demo Plugin
// Description: A wxWidgets wxDialog to display Active Captain POI Details
// Owner: twocanplugin@hotmail.com
// Date: 28/04/2026
// Version History: 
// 1.0 Initial Release


#include "demo_activecaptain.h"

// Constructor and destructor implementation
DemoActiveCaptain::DemoActiveCaptain(const ActiveCaptainPOI& poiDetails, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) :
	DemoActiveCaptainBase(parent, id, title, pos, size, style) {
	poi = poiDetails;
}

DemoActiveCaptain::~DemoActiveCaptain() {
}

void DemoActiveCaptain::OnInit(wxInitDialogEvent& event) {

	labelID->SetLabel(poi.id);
	labelName->SetLabel(poi.name);
	labelLatitude->SetLabel(toSDMM_PlugIn(1, poi.latitude, true));
	labelLongitude->SetLabel(toSDMM_PlugIn(2, poi.longitude, true));
	textInformation->SetValue(poi.notes);
	switch (poi.type) {
	case Category::Marina:
		bitmapType->SetBitmap(bitmapMarina);
		break;
	case Category::Anchorage:
		bitmapType->SetBitmap(bitmapAnchorage);
		break;
	case Category::Business:
		bitmapType->SetBitmap(bitmapBusiness);
		break;
	case Category::BoatRamp:
		bitmapType->SetBitmap(bitmapBoatRamp);
		break;
	case Category::Bridge:
		bitmapType->SetBitmap(bitmapBridge);
		break;
	case Category::Dam:
		bitmapType->SetBitmap(bitmapDam);
		break;
	case Category::Ferry:
		bitmapType->SetBitmap(bitmapFerry);
		break;
	case Category::Hazard:
		bitmapType->SetBitmap(bitmapHazard);
		break;
	case Category::Inlet:
		bitmapType->SetBitmap(bitmapInlet);
		break;
	case Category::Lock:
		bitmapType->SetBitmap(bitmapLock);
		break;
	default: // Unaware of any POI's that have a different type
		bitmapType->SetBitmap(bitmapDefault);
		break;
	}
	SetMinSize(wxSize(300, 400));
	Layout();
	Fit();
	CenterOnParent();

}

void DemoActiveCaptain::OnOK(wxCommandEvent& event) {
	EndModal(wxID_OK);
}