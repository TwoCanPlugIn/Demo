///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/slider.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/wizard.h>
#include <wx/dynarray.h>
WX_DEFINE_ARRAY_PTR( wxWizardPageSimple*, WizardPages );

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DemoWizardBase
///////////////////////////////////////////////////////////////////////////////
class DemoWizardBase : public wxWizard
{
	private:

	protected:
		wxCheckBox* checkBoxBoolean;
		wxStaticText* labelInteger;
		wxSlider* sliderInteger;
		wxStaticText* labelString;
		wxTextCtrl* textString;

		// Virtual event handlers, override them in your derived class
		virtual void OnInit( wxInitDialogEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxWizardEvent& event ) { event.Skip(); }
		virtual void OnFinished( wxWizardEvent& event ) { event.Skip(); }
		virtual void OnCheckBoolean( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSliderChanged( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnTextChanged( wxCommandEvent& event ) { event.Skip(); }


	public:

		DemoWizardBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Demo Plugin Install"), const wxBitmap& bitmap = wxNullBitmap, const wxPoint& pos = wxDefaultPosition, long style = wxDEFAULT_DIALOG_STYLE );
		WizardPages m_pages;

		~DemoWizardBase();

};

