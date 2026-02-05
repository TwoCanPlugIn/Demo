///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "demo_ui_base.h"

///////////////////////////////////////////////////////////////////////////

DemoWizardBase::DemoWizardBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxBitmap& bitmap, const wxPoint& pos, long style )
{
	this->Create( parent, id, title, bitmap, pos, style );

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxWizardPageSimple* DemoWizardPageOne = new wxWizardPageSimple( this );
	m_pages.Add( DemoWizardPageOne );

	wxBoxSizer* sizerPageOne;
	sizerPageOne = new wxBoxSizer( wxVERTICAL );

	checkBoxBoolean = new wxCheckBox( DemoWizardPageOne, wxID_ANY, _("Some Boolean Value"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerPageOne->Add( checkBoxBoolean, 0, wxALL, 5 );

	labelInteger = new wxStaticText( DemoWizardPageOne, wxID_ANY, _("Some Integer Value"), wxDefaultPosition, wxDefaultSize, 0 );
	labelInteger->Wrap( -1 );
	sizerPageOne->Add( labelInteger, 0, wxALL, 5 );

	sliderInteger = new wxSlider( DemoWizardPageOne, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_MIN_MAX_LABELS|wxSL_VALUE_LABEL );
	sizerPageOne->Add( sliderInteger, 0, wxALL|wxEXPAND, 5 );

	labelString = new wxStaticText( DemoWizardPageOne, wxID_ANY, _("Some String Value"), wxDefaultPosition, wxDefaultSize, 0 );
	labelString->Wrap( -1 );
	sizerPageOne->Add( labelString, 0, wxALL, 5 );

	textString = new wxTextCtrl( DemoWizardPageOne, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sizerPageOne->Add( textString, 0, wxALL|wxEXPAND, 5 );


	DemoWizardPageOne->SetSizer( sizerPageOne );
	DemoWizardPageOne->Layout();
	sizerPageOne->Fit( DemoWizardPageOne );

	this->Centre( wxBOTH );

	for ( unsigned int i = 1; i < m_pages.GetCount(); i++ )
	{
		m_pages.Item( i )->SetPrev( m_pages.Item( i - 1 ) );
		m_pages.Item( i - 1 )->SetNext( m_pages.Item( i ) );
	}

	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DemoWizardBase::OnInit ) );
	this->Connect( wxID_ANY, wxEVT_WIZARD_CANCEL, wxWizardEventHandler( DemoWizardBase::OnCancel ) );
	this->Connect( wxID_ANY, wxEVT_WIZARD_FINISHED, wxWizardEventHandler( DemoWizardBase::OnFinished ) );
	checkBoxBoolean->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DemoWizardBase::OnCheckBoolean ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( DemoWizardBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( DemoWizardBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( DemoWizardBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( DemoWizardBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( DemoWizardBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( DemoWizardBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( DemoWizardBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( DemoWizardBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( DemoWizardBase::OnSliderChanged ), NULL, this );
	textString->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DemoWizardBase::OnTextChanged ), NULL, this );
}

DemoWizardBase::~DemoWizardBase()
{
	m_pages.Clear();
}

DemoSettingsBase::DemoSettingsBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* sizerDialog;
	sizerDialog = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerSettings;
	sizerSettings = new wxBoxSizer( wxVERTICAL );

	checkBoxBoolean = new wxCheckBox( this, wxID_ANY, _("Some Boolean Value"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerSettings->Add( checkBoxBoolean, 0, wxALL, 5 );

	labelInteger = new wxStaticText( this, wxID_ANY, _("Some Integer Value"), wxDefaultPosition, wxDefaultSize, 0 );
	labelInteger->Wrap( -1 );
	sizerSettings->Add( labelInteger, 0, wxALL, 5 );

	sliderInteger = new wxSlider( this, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_MIN_MAX_LABELS|wxSL_VALUE_LABEL );
	sizerSettings->Add( sliderInteger, 0, wxALL|wxEXPAND, 5 );

	labelString = new wxStaticText( this, wxID_ANY, _("Some String Value"), wxDefaultPosition, wxDefaultSize, 0 );
	labelString->Wrap( -1 );
	sizerSettings->Add( labelString, 0, wxALL, 5 );

	textString = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sizerSettings->Add( textString, 0, wxALL|wxEXPAND, 5 );


	sizerDialog->Add( sizerSettings, 1, wxEXPAND, 5 );

	wxBoxSizer* sizerButtons;
	sizerButtons = new wxBoxSizer( wxHORIZONTAL );


	sizerButtons->Add( 0, 0, 1, wxEXPAND, 5 );

	buttonOK = new wxButton( this, wxID_ANY, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerButtons->Add( buttonOK, 0, wxALL, 5 );

	buttonApply = new wxButton( this, wxID_ANY, _("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerButtons->Add( buttonApply, 0, wxALL, 5 );

	buttonCancel = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerButtons->Add( buttonCancel, 0, wxALL, 5 );


	sizerDialog->Add( sizerButtons, 0, wxEXPAND, 5 );


	this->SetSizer( sizerDialog );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DemoSettingsBase::OnInit ) );
	checkBoxBoolean->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DemoSettingsBase::OnCheckBoolean ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( DemoSettingsBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( DemoSettingsBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( DemoSettingsBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( DemoSettingsBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( DemoSettingsBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( DemoSettingsBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( DemoSettingsBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( DemoSettingsBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( DemoSettingsBase::OnSliderChanged ), NULL, this );
	textString->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DemoSettingsBase::OnTextChanged ), NULL, this );
	buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DemoSettingsBase::OnOK ), NULL, this );
	buttonApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DemoSettingsBase::OnApply ), NULL, this );
	buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DemoSettingsBase::OnCancel ), NULL, this );
}

DemoSettingsBase::~DemoSettingsBase()
{
}

DemoToolboxBase::DemoToolboxBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* sizerToolbox;
	sizerToolbox = new wxBoxSizer( wxVERTICAL );

	checkBoxBoolean = new wxCheckBox( this, wxID_ANY, _("Some Boolean Value"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerToolbox->Add( checkBoxBoolean, 0, wxALL, 5 );

	labelInteger = new wxStaticText( this, wxID_ANY, _("Some Integer Value"), wxDefaultPosition, wxDefaultSize, 0 );
	labelInteger->Wrap( -1 );
	sizerToolbox->Add( labelInteger, 0, wxALL, 5 );

	sliderInteger = new wxSlider( this, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_MIN_MAX_LABELS|wxSL_VALUE_LABEL );
	sizerToolbox->Add( sliderInteger, 0, wxALL|wxEXPAND, 5 );

	labelString = new wxStaticText( this, wxID_ANY, _("Some String Value"), wxDefaultPosition, wxDefaultSize, 0 );
	labelString->Wrap( -1 );
	sizerToolbox->Add( labelString, 0, wxALL, 5 );

	textString = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sizerToolbox->Add( textString, 0, wxALL|wxEXPAND, 5 );


	this->SetSizer( sizerToolbox );
	this->Layout();

	// Connect Events
	checkBoxBoolean->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DemoToolboxBase::OnCheckBoolean ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( DemoToolboxBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( DemoToolboxBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( DemoToolboxBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( DemoToolboxBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( DemoToolboxBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( DemoToolboxBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( DemoToolboxBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( DemoToolboxBase::OnSliderChanged ), NULL, this );
	sliderInteger->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( DemoToolboxBase::OnSliderChanged ), NULL, this );
	textString->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DemoToolboxBase::OnTextChanged ), NULL, this );
}

DemoToolboxBase::~DemoToolboxBase()
{
}
