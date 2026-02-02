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

#ifndef DEMO_WIZARD_H
#define DEMO_WIZARD_H

// wxWidgets Precompiled Headers
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif

// The wxWizard base class from which we are derived
// Note wxFormBuilder was used to generate the UI
#include "demo_wizardbase.h"

class DemoWizard : public DemoWizardBase {
	
public:
	DemoWizard(wxWindow* parent);
	~DemoWizard();
	
protected:
	// Overridden methods from the base class
	void OnInit(wxInitDialogEvent& event) override;
	void OnCancel(wxWizardEvent& event) override;
	void OnFinished(wxWizardEvent& event) override;

protected:
	
private:
	
};

#endif
