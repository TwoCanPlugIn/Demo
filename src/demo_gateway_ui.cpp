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


// Project: Gateway Plugin
// Description: Derived from Demo plugin, a simple bi-directional NMEA 0183 <-> NMEA 2000 gateway
// Owner: twocanplugin@hotmail.com
// Date: 17/04/2026
// Version History: 
// 1.0 Initial Release

// Note to self, wxFormbuilder doesn't add the following events to wxGrid
// virtual void OnCellChanging(wxGridEvent& event) { event.Skip(); }
// gridPGN->Connect(wxEVT_GRID_CELL_CHANGING, wxGridEventHandler(DemoBridgeUIBase::OnCellChanging), NULL, this);

#include "demo_gateway_ui.h"

// Constructor and destructor implementation
DemoGatewayUI::DemoGatewayUI(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : 
	DemoGatewayUIBase(parent, id, title, pos, size, style) {

	// Generate getters to populate the wxGrid cell values from the global variables
	for (auto& [pgn, ptr] : variableBindings) {
		gridRowGetter.emplace(pgn, [ptr]() { return (*ptr); });
	}

	// Generate setters to set global variables from wxGrid cell values
	for (auto& [pgn, ptr] : variableBindings) {
		gridRowSetter.emplace(pgn, [ptr](const ConversionType& v) { *ptr = v; });
	}
}

DemoGatewayUI::~DemoGatewayUI() {
}

void DemoGatewayUI::OnInit(wxInitDialogEvent& event) {
	// Set a fixed size for the dialog, should probably use FromDIP
	SetMinSize(wxSize(800, 700));
	SetMaxSize(wxSize(800, 700));

	// Set the number of columns and rows, note initial rows set from wxFormbuilder
	gridPGN->ClearGrid();
	gridPGN->AppendRows(variableBindings.size() - gridPGN->GetNumberRows());
	
	// Set the grid labels, should consider localization
	gridPGN->SetColLabelValue(0, "Description");
	gridPGN->SetColLabelValue(1, "Convert to NMEA 0183");
	gridPGN->SetColLabelValue(2, "Convert to NMEA 2000");
	gridPGN->SetCornerLabelValue("PGN");
	
	// Configure columns 1 and 2 as boolean checkboxes
	int row = 0;
	for (auto [pgn, description] : supportedConversions) {
		gridPGN->SetRowLabelValue(row, std::to_string(pgn));
		gridPGN->SetCellValue(row, 0, description);
		gridPGN->SetCellRenderer(row, 1, new wxGridCellBoolRenderer);
		gridPGN->SetCellEditor(row, 1, new wxGridCellBoolEditor);
		gridPGN->SetCellRenderer(row, 2, new wxGridCellBoolRenderer);
		gridPGN->SetCellEditor(row, 2, new wxGridCellBoolEditor);

		// Set the values for each cell. The global variables were associated
		// with the rows previously in the variableBindings map.
		auto it = gridRowGetter.find(pgn);
		if (it != gridRowGetter.end()) {
			// Set the cell value with a non-empty string to "check" a binary value
			if (it->second() == ConversionType::NMEA0183) {
				gridPGN->SetCellValue(row, 1, "1");
			}
			else if (it->second() == ConversionType::NMEA2000) {
				gridPGN->SetCellValue(row, 2, "1");
			}
		}
		row++;
	}
	
	// Populate Network Interface Combo boxes
	// Note the "opaque" driver handle is persisted as client data so we don't
	// later have to perform a reverse lookup using the description to obtain the handle
	std::unordered_map<DriverHandle, std::string>networkInterfaces;

	networkInterfaces = FindOutboundConnection("nmea0183");
	int index;
	for (const auto &[driver, description] : networkInterfaces) {
		index = choiceNMEA0183->Append(description);
		choiceNMEA0183->SetClientObject(index, new wxStringClientData(driver));
		if (g_nmea0183Driver == driver) {
			choiceNMEA0183->SetSelection(index);
		}
	}
	
	networkInterfaces = FindOutboundConnection("nmea2000");

	for (const auto &[driver, description] : networkInterfaces) {
		index = choiceNMEA2000->Append(description);
		choiceNMEA2000->SetClientObject(index, new wxStringClientData(driver));
		if (g_nmea2000Driver == driver) {
			choiceNMEA2000->SetSelection(index);
		}
	}
	
	Layout();
	Fit();
	CenterOnParent();

	// After we've fitted in everything adjust the Grid column widths
	// More reliable way to determine if the vertical scroll bar is shown
	// when the grid is initialized rather than gridPGN->HasScrollbar(wxVSCROLL);
	bool needVScroll =
		gridPGN->GetNumberRows() * gridPGN->GetDefaultRowSize()
	> gridPGN->GetGridWindow()->GetClientSize().GetHeight();

	int gridWidth =
		gridPGN->GetClientSize().GetWidth() - gridPGN->GetRowLabelSize() -
		(needVScroll ?
			wxSystemSettings::GetMetric(wxSYS_VSCROLL_X) : 0);
	gridPGN->SetColSize(0, gridWidth / 3);
	gridPGN->SetColSize(1, gridPGN->GetColSize(0));
	gridPGN->SetColSize(2, gridPGN->GetColSize(0));
}

// Save our settings
void DemoGatewayUI::OnOK(wxCommandEvent& event) {
	for (int row = 0; row < gridPGN->GetNumberRows(); ++row) {
		long label;
		gridPGN->GetRowLabelValue(row).ToLong(&label);
		// Default value if neither checkbox is ticked
		ConversionType value = ConversionType::None;
	
		// Check if either column 1 or column 2 are checked
		if (!gridPGN->GetCellValue(row, 1).IsEmpty()) {
			value = ConversionType::NMEA0183;
		}
		else if (!gridPGN->GetCellValue(row, 2).IsEmpty()) {
			value = ConversionType::NMEA2000;
		}

		// Call the setter lambda function to save the global variable value
		auto it = gridRowSetter.find(static_cast<int>(label));
		if (it != gridRowSetter.end()) {
			it->second(value);   
		}
	}

	// Save network connection settings
	// NMEA 0183
	if (choiceNMEA0183->GetSelection() != wxNOT_FOUND) {
		wxStringClientData* data = static_cast<wxStringClientData*>(
			choiceNMEA0183->GetClientObject(choiceNMEA0183->GetSelection()));
		if (data) {
			g_nmea0183Driver = data->GetData();
		}
	}
	else {
		g_nmea0183Driver = wxEmptyString;
	}

	// NMEA 2000 Interfaces
	if (choiceNMEA2000->GetSelection() != wxNOT_FOUND) {
		wxStringClientData* data = static_cast<wxStringClientData*>(
			choiceNMEA2000->GetClientObject(choiceNMEA2000->GetSelection()));
		if (data) {
			g_nmea2000Driver = data->GetData();
		}
	}
	else {
		g_nmea2000Driver = wxEmptyString;
	}
	EndModal(wxID_OK);
}

void DemoGatewayUI::OnCancel(wxCommandEvent& event) {
	EndModal(wxID_CANCEL);
}

void DemoGatewayUI::OnInterfaceAChanged(wxCommandEvent& event) {
	// Nothing to do
}
void DemoGatewayUI::OnInterfaceBChanged(wxCommandEvent& event) {
	// Nothing to do
}

void DemoGatewayUI::OnCellChanged(wxGridEvent& event) {
	auto col = event.GetCol();
	auto row = event.GetRow();
	auto checked = event.GetString();
	// Nothing to do, not sure why I wanted the old value?
}

// Note, refer to wxFormbuilder comment above
// Treat the columns as radio buttons, only one can be checked at a time.
void DemoGatewayUI::OnCellChanging(wxGridEvent& event) {
	auto col = event.GetCol();
	auto row = event.GetRow();
	auto checked = event.GetString();
	// If we're checked, make sure the other cell is unchecked
	if (!checked.IsEmpty()) {
		gridPGN->SetCellValue(row, 3 - col, wxEmptyString);
	}
}

// Handle double right click to select/deselect all rows of a column
void DemoGatewayUI::OnLabelClicked(wxGridEvent& event) {
	int col = event.GetCol();
	toggle[col] = !toggle[col];
	for (int i = 0; i < gridPGN->GetNumberRows(); i++) {
		gridPGN->SetCellValue(i, col, toggle[col] ? "1" : "");
		// toggle the other column
		if (toggle[col]) {
			toggle[3 - col] = false;
			gridPGN->SetCellValue(i, 3 - col, "");
		}
	}
}

// Helper function to find required driver handles. Modified slightly from
// original demo plugin so we can populate the combo boxes 
std::unordered_map<DriverHandle, std::string> DemoGatewayUI::FindOutboundConnection(const std::string& connectionType) {
	std::unordered_map<DriverHandle, std::string> networkInterfaces;

	// Iterate through all of the OpenCPN connections

	for (const auto& driver : GetActiveDrivers()) {
		const auto& attributes = GetAttributes(driver);

		auto protocolIterator = attributes.find("protocol");
		auto directionIterator = attributes.find("ioDirection");

		if (protocolIterator != attributes.end() && directionIterator != attributes.end()) {
			// Found a connection matching the required connection and io direction
			if ((connectionType == protocolIterator->second) && ((directionIterator->second == "OUT")
				|| (directionIterator->second == "IN/OUT"))) {

				auto description = attributes.find("userComment");
				if (description != attributes.end()) {
					// The user added a comment to the connection
					if (description->second.length() != 0) {
						networkInterfaces.emplace(driver, description->second);
					}
					else {
						// No user comment field, so construct one
						auto convert = [](const std::string& text) {
							const std::string delimiter = "!@!";
							auto position = text.find(delimiter);
							// Split the "opaque" driver handle
							return text.substr(0, position) + " on " + text.substr(position + delimiter.size());
							};
						networkInterfaces.emplace(driver, convert(driver));
					}
				}
			}
		}
	}
	return networkInterfaces;
}

// Unused, Helper function to perform a "reverse" lookup on the std:unordered map
int DemoGatewayUI::GetKeyFromValue(const std::unordered_map<int, std::string>& foobar,
	const std::string& value) {
	for (const auto& [key, val] : foobar) {
		if (val == value)
			return key;
	}
	return -1; 
}