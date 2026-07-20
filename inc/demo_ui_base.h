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
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/statbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/wizard.h>
#include <wx/dynarray.h>
WX_DEFINE_ARRAY_PTR( wxWizardPageSimple*, WizardPages );
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DemoWizardBase
///////////////////////////////////////////////////////////////////////////////
class DemoWizardBase : public wxWizard
{
	private:

	protected:
		wxScrolledWindow* scrolledWindow;
		wxCheckBox* checkMarina;
		wxCheckBox* checkAnchorage;
		wxCheckBox* checkHazard;
		wxCheckBox* checkBusiness;
		wxCheckBox* checkBoatRamp;
		wxCheckBox* checkBridge;
		wxCheckBox* checkDam;
		wxCheckBox* checkFerry;
		wxCheckBox* checkLock;
		wxStaticBitmap* bitmapGarmin;
		wxStaticText* labelDisclaimer;

		// Virtual event handlers, override them in your derived class
		virtual void OnInit( wxInitDialogEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxWizardEvent& event ) { event.Skip(); }
		virtual void OnFinished( wxWizardEvent& event ) { event.Skip(); }


	public:

		DemoWizardBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Garmin Active Captain PlugIn"), const wxBitmap& bitmap = wxNullBitmap, const wxPoint& pos = wxDefaultPosition, long style = wxDEFAULT_DIALOG_STYLE );
		WizardPages m_pages;

		~DemoWizardBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DemoSettingsBase
///////////////////////////////////////////////////////////////////////////////
class DemoSettingsBase : public wxDialog
{
	private:

	protected:
		wxScrolledWindow* scrolledWindow;
		wxCheckBox* checkMarina;
		wxCheckBox* checkAnchorage;
		wxCheckBox* checkHazard;
		wxCheckBox* checkBusiness;
		wxCheckBox* checkBoatRamp;
		wxCheckBox* checkBridge;
		wxCheckBox* checkDam;
		wxCheckBox* checkFerry;
		wxCheckBox* checkLock;
		wxStaticBitmap* bitmapGarmin;
		wxStaticText* labelDisclaimer;
		wxButton* buttonOK;
		wxButton* buttonCancel;

		// Virtual event handlers, override them in your derived class
		virtual void OnInit( wxInitDialogEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }


	public:

		DemoSettingsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Demo Plugin Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 495,398 ), long style = wxDEFAULT_DIALOG_STYLE );

		~DemoSettingsBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DemoToolboxBase
///////////////////////////////////////////////////////////////////////////////
class DemoToolboxBase : public wxPanel
{
	private:

	protected:
		wxScrolledWindow* scrolledWindow;
		wxCheckBox* checkMarina;
		wxCheckBox* checkAnchorage;
		wxCheckBox* checkHazard;
		wxCheckBox* checkBusiness;
		wxCheckBox* checkBoatRamp;
		wxCheckBox* checkBridge;
		wxCheckBox* checkDam;
		wxCheckBox* checkFerry;
		wxCheckBox* checkLock;
		wxStaticBitmap* bitmapGarmin;
		wxStaticText* labelDisclaimer;

	public:

		DemoToolboxBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,345 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~DemoToolboxBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DemoActiveCaptainBase
///////////////////////////////////////////////////////////////////////////////
class DemoActiveCaptainBase : public wxDialog
{
	private:

	protected:
		wxStaticText* labelID;
		wxStaticText* labelName;
		wxStaticText* labelLatitude;
		wxStaticText* labelLongitude;
		wxStaticBitmap* bitmapType;
		wxStaticText* labelInformation;
		wxTextCtrl* textInformation;
		wxButton* buttonOK;

		// Virtual event handlers, override them in your derived class
		virtual void OnInit( wxInitDialogEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }


	public:

		DemoActiveCaptainBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 427,444 ), long style = wxDEFAULT_DIALOG_STYLE );

		~DemoActiveCaptainBase();

};

