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
// Description: A Simple Dashboard for the AUI demo
// Owner: twocanplugin@hotmail.com
// Date: 10/01/2026
// Version History: 
// 1.0 Initial Release

#include "demo_dash.h"

#include <wx/dcbuffer.h>
#include <cmath>

// Quick and dirty compass
Compass::Compass(wxWindow* parent): wxPanel(parent) {

    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &Compass::OnPaint, this);
    Bind(wxEVT_SIZE, &Compass::OnSize, this);

    DrawBackground();
}

void Compass::SetHeading(double degrees) {
    heading = degrees;
    Refresh();
}

void Compass::SetNightMode(bool mode) {
    nightMode = mode;
    DrawBackground();
    Refresh();
}

void Compass::OnPaint(wxPaintEvent&) {
    // Use double-buffered paint to reduce flickering
    wxAutoBufferedPaintDC dc(this);

    // Blit the background, no need to compute everytime
    if (backgroundBitmap.IsOk()) {
        dc.DrawBitmap(backgroundBitmap, 0, 0);
    }
    else {
        dc.Clear();
    }

    DrawNeedle(dc);
}

void Compass::OnSize(wxSizeEvent& evt) {
    DrawBackground();
    Refresh();
    evt.Skip();
}

void Compass::DrawBackground() {
    wxSize size = GetClientSize();
    if (size.x <= 0 || size.y <= 0) {
        return;
    }

    backgroundBitmap = wxBitmap(size);
    wxMemoryDC dc(backgroundBitmap);
    dc.SetBrush(this->GetParent()->GetBackgroundColour());
    dc.Clear();
    
    int width = size.GetWidth();
    int height = size.GetHeight();
    int radius = std::min(width, height) / 2 - 10;
    int centreX = width / 2;
    int centreY = height / 2;

    // Outer circle
    dc.SetPen(nightMode ? *wxWHITE_PEN : *wxBLACK_PEN);
    dc.DrawCircle(centreX, centreY, radius);
    
    // Tick marks
    for (int degrees = 0; degrees < 360; degrees += 10) {
        double radians = wxDegToRad(degrees);
        
        // Larger tick marks every 30 degrees
        int inner = (degrees % 30 == 0) ? radius - 15 : radius - 8;
        int outer = radius;
        
        int x1 = centreX + inner * std::sin(radians);
        int y1 = centreY - inner * std::cos(radians);
        int x2 = centreX + outer * std::sin(radians);
        int y2 = centreY - outer * std::cos(radians);
        dc.DrawLine(x1, y1, x2, y2);
    }
    
    // Cardinal labels
    dc.SetFont(wxFontInfo(12).Bold());
    dc.SetTextForeground(nightMode ? wxColour(*wxWHITE) : wxColour(*wxBLACK));
       
    dc.DrawText("N", centreX - 6, centreY - radius + 5);
    dc.DrawText("S", centreX - 6, centreY + radius - 20);
    dc.DrawText("E", centreX + radius - 20, centreY - 6);
    dc.DrawText("W", centreX - radius + 5, centreY - 6);
}

void Compass::DrawNeedle(wxDC& dc) {
    if (std::isnan(heading)) {
        return;
    }
    
    wxSize size = GetClientSize();
    int width = size.GetWidth();
    int height = size.GetHeight();
    
    int radius = std::min(width, height) / 2 - 10;
    int centreX = width / 2;
    int centreY = height / 2;
    
    double radians = wxDegToRad(heading);
    int needLength = radius - 20;
    
    int tipX = centreX + needLength * std::sin(radians);
    int tipY = centreY - needLength * std::cos(radians);
    
    // North
    dc.SetPen(wxPen(*wxRED, 3));
    dc.DrawLine(centreX, centreY, tipX, tipY);
    
    // South
    dc.SetPen(nightMode ? wxPen(*wxWHITE, 3) : wxPen(*wxBLACK, 3));
    dc.DrawLine(centreX, centreY, centreX - (needLength / 2) * std::sin(radians),
        centreY + (needLength / 2) * std::cos(radians));
}

// Constructor and destructor implementation
DemoDashboard::DemoDashboard( wxWindow* parent) : DemoDashboardBase(parent) {
    compass = std::make_unique<Compass>(panelCompass);

    // Need to set the size of the panel, so the compass size is not 0,0
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(compass.get(), 1, wxEXPAND | wxALL, 0);

    panelCompass->SetSizer(sizer);
    panelCompass->Layout();
}

DemoDashboard::~DemoDashboard() {
}

// Event Handlers
void DemoDashboard::OnButtonClose(wxCommandEvent& event) {

    if (wxTheClipboard->Open()) {
        wxTheClipboard->Clear();
        wxTheClipboard->Close();
    }

	// Mimic the behaviour of the x button on the title bar
	// Required so that we can synch the plugin toolbar button state if we close 
    // using the close button instead of the x button. AUI internal wiring...
	wxAuiManager* auiManager = wxAuiManager::GetManager(this);

    // If AUI Manager is gone, OpenCPN is shutting down so do nothing
    if (!auiManager || !auiManager->GetManagedWindow()) {
        return;
    }

	wxAuiPaneInfo& pane = auiManager->GetPane(this);

	// Raise the Pane Close Event
	wxAuiManagerEvent evt(wxEVT_AUI_PANE_CLOSE);
	evt.SetPane(&pane);
	evt.SetManager(auiManager);

	wxWindow* auiWindow = auiManager->GetManagedWindow();
    // Again, if AUI Window is gone, OpenCPN is shutting down so do nothing
    if (!auiWindow || !auiWindow->GetEventHandler()) {
        return;
    }

    // Send the Pane Close event
	auiWindow->GetEventHandler()->ProcessEvent(evt);

	// As it's our code, the plugin doesn't veto the event, but just in case...
	if (!evt.GetVeto())	{
		auiManager->ClosePane(pane);
		auiManager->Update();
	}
}

// Copy some stuff to the clipboard
void DemoDashboard::OnButtonCopy(wxCommandEvent& event) {
	if (wxTheClipboard->Open()) {
		wxString text = wxString::Format("Latitude: %s\nLongitude: %s\nWind Speed: %s\nWind Angle: %.2f",
			latitudeValue->GetValue(), longitudeValue->GetValue(),
			windSpeedValue->GetValue(), windAngleValue->GetValue());
		wxTheClipboard->SetData(new wxTextDataObject(text));
		wxTheClipboard->Close();
	}
}

// Setters
void DemoDashboard::SetPosition(const double& latitude, const double& longitude) {
	latitudeValue->SetValue(toSDMM_PlugIn(1, latitude));
	longitudeValue->SetValue(toSDMM_PlugIn(2, longitude));
}

void DemoDashboard::SetWindAngle(const double& angle) {
	windAngleValue->SetValue(wxString::Format("%.2f", angle));
}

void DemoDashboard::SetWindSpeed(const double& speed) {
	windSpeedValue->SetValue(wxString::Format("%.1f %s", 
		toUsrWindSpeed_Plugin(speed), getUsrSpeedUnit_Plugin()));
}

void DemoDashboard::SetMagneticHeading(const double& heading) {
    compass->SetHeading(heading);
}

void DemoDashboard::SetNightMode(bool mode) {
    
    // Apply the OpenCPN System colours
    DimeWindow(this);
    
    wxColour background = GetBackgroundColour();
    
    // Iterate the child controls and set their colours
    // Note that I'm using my own foreground colours
    wxWindowList& children = this->GetChildren();

    for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it) {
        wxWindow* child = *it;
        child->SetBackgroundColour(background);
        child->SetForegroundColour(mode ? *wxWHITE : *wxBLACK);
    }

    // Similarly set the colour schem for the compass
    compass->SetNightMode(mode);

    // Force a repaint with the new colour scheme
    Refresh();
}