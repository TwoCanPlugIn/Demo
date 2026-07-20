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

	wxStaticBoxSizer* SizerCheckBoxes;
	SizerCheckBoxes = new wxStaticBoxSizer( new wxStaticBox( DemoWizardPageOne, wxID_ANY, _("Select Points of Interest (POI)") ), wxVERTICAL );

	scrolledWindow = new wxScrolledWindow( SizerCheckBoxes->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
	scrolledWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* sizerScrollWindow;
	sizerScrollWindow = new wxBoxSizer( wxVERTICAL );

	checkMarina = new wxCheckBox( scrolledWindow, wxID_ANY, _("Marina"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkMarina, 0, wxALL, 5 );

	checkAnchorage = new wxCheckBox( scrolledWindow, wxID_ANY, _("Anchorage"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkAnchorage, 0, wxALL, 5 );

	checkHazard = new wxCheckBox( scrolledWindow, wxID_ANY, _("Hazard"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkHazard, 0, wxALL, 5 );

	checkBusiness = new wxCheckBox( scrolledWindow, wxID_ANY, _("Business"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkBusiness, 0, wxALL, 5 );

	checkBoatRamp = new wxCheckBox( scrolledWindow, wxID_ANY, _("Boat Ramp"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkBoatRamp, 0, wxALL, 5 );

	checkBridge = new wxCheckBox( scrolledWindow, wxID_ANY, _("Bridge"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkBridge, 0, wxALL, 5 );

	checkDam = new wxCheckBox( scrolledWindow, wxID_ANY, _("Dam"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkDam, 0, wxALL, 5 );

	checkFerry = new wxCheckBox( scrolledWindow, wxID_ANY, _("Ferry"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkFerry, 0, wxALL, 5 );

	checkLock = new wxCheckBox( scrolledWindow, wxID_ANY, _("Lock"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkLock, 0, wxALL, 5 );


	scrolledWindow->SetSizer( sizerScrollWindow );
	scrolledWindow->Layout();
	sizerScrollWindow->Fit( scrolledWindow );
	SizerCheckBoxes->Add( scrolledWindow, 1, wxEXPAND | wxALL, 5 );


	sizerPageOne->Add( SizerCheckBoxes, 1, wxEXPAND, 5 );

	wxBoxSizer* sizerDisclaimer;
	sizerDisclaimer = new wxBoxSizer( wxHORIZONTAL );

	bitmapGarmin = new wxStaticBitmap( DemoWizardPageOne, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizerDisclaimer->Add( bitmapGarmin, 0, wxALL, 5 );

	wxBoxSizer* sizerLabels;
	sizerLabels = new wxBoxSizer( wxVERTICAL );

	labelDisclaimer = new wxStaticText( DemoWizardPageOne, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	labelDisclaimer->Wrap( -1 );
	sizerLabels->Add( labelDisclaimer, 0, wxALL, 5 );


	sizerDisclaimer->Add( sizerLabels, 1, wxEXPAND, 5 );


	sizerPageOne->Add( sizerDisclaimer, 0, wxEXPAND, 5 );


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

	wxStaticBoxSizer* SizerCheckBoxes;
	SizerCheckBoxes = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Select Points of Interest (POI)") ), wxVERTICAL );

	scrolledWindow = new wxScrolledWindow( SizerCheckBoxes->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
	scrolledWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* sizerScrollWindow;
	sizerScrollWindow = new wxBoxSizer( wxVERTICAL );

	checkMarina = new wxCheckBox( scrolledWindow, wxID_ANY, _("Marina"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkMarina, 0, wxALL, 5 );

	checkAnchorage = new wxCheckBox( scrolledWindow, wxID_ANY, _("Anchorage"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkAnchorage, 0, wxALL, 5 );

	checkHazard = new wxCheckBox( scrolledWindow, wxID_ANY, _("Hazard"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkHazard, 0, wxALL, 5 );

	checkBusiness = new wxCheckBox( scrolledWindow, wxID_ANY, _("Business"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkBusiness, 0, wxALL, 5 );

	checkBoatRamp = new wxCheckBox( scrolledWindow, wxID_ANY, _("Boat Ramp"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkBoatRamp, 0, wxALL, 5 );

	checkBridge = new wxCheckBox( scrolledWindow, wxID_ANY, _("Bridge"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkBridge, 0, wxALL, 5 );

	checkDam = new wxCheckBox( scrolledWindow, wxID_ANY, _("Dam"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkDam, 0, wxALL, 5 );

	checkFerry = new wxCheckBox( scrolledWindow, wxID_ANY, _("Ferry"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkFerry, 0, wxALL, 5 );

	checkLock = new wxCheckBox( scrolledWindow, wxID_ANY, _("Lock"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkLock, 0, wxALL, 5 );


	scrolledWindow->SetSizer( sizerScrollWindow );
	scrolledWindow->Layout();
	sizerScrollWindow->Fit( scrolledWindow );
	SizerCheckBoxes->Add( scrolledWindow, 1, wxEXPAND | wxALL, 5 );


	sizerDialog->Add( SizerCheckBoxes, 1, wxEXPAND, 5 );

	wxBoxSizer* sizerDisclaimer;
	sizerDisclaimer = new wxBoxSizer( wxHORIZONTAL );

	bitmapGarmin = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizerDisclaimer->Add( bitmapGarmin, 0, wxALL, 5 );

	wxBoxSizer* sizerLabels;
	sizerLabels = new wxBoxSizer( wxVERTICAL );

	labelDisclaimer = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	labelDisclaimer->Wrap( -1 );
	sizerLabels->Add( labelDisclaimer, 0, wxALL, 5 );


	sizerDisclaimer->Add( sizerLabels, 1, wxEXPAND, 5 );


	sizerDialog->Add( sizerDisclaimer, 0, wxEXPAND, 5 );

	wxBoxSizer* sizerButtons;
	sizerButtons = new wxBoxSizer( wxHORIZONTAL );


	sizerButtons->Add( 0, 0, 1, wxEXPAND, 5 );

	buttonOK = new wxButton( this, wxID_ANY, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerButtons->Add( buttonOK, 0, wxALL, 5 );

	buttonCancel = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerButtons->Add( buttonCancel, 0, wxALL, 5 );


	sizerDialog->Add( sizerButtons, 0, wxEXPAND, 5 );


	this->SetSizer( sizerDialog );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DemoSettingsBase::OnInit ) );
	buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DemoSettingsBase::OnOK ), NULL, this );
	buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DemoSettingsBase::OnCancel ), NULL, this );
}

DemoSettingsBase::~DemoSettingsBase()
{
}

DemoToolboxBase::DemoToolboxBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* sizerPanel;
	sizerPanel = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* SizerCheckBoxes;
	SizerCheckBoxes = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Select Points of Interest (POI)") ), wxVERTICAL );

	scrolledWindow = new wxScrolledWindow( SizerCheckBoxes->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
	scrolledWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* sizerScrollWindow;
	sizerScrollWindow = new wxBoxSizer( wxVERTICAL );

	checkMarina = new wxCheckBox( scrolledWindow, wxID_ANY, _("Marina"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkMarina, 0, wxALL, 5 );

	checkAnchorage = new wxCheckBox( scrolledWindow, wxID_ANY, _("Anchorage"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkAnchorage, 0, wxALL, 5 );

	checkHazard = new wxCheckBox( scrolledWindow, wxID_ANY, _("Hazard"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkHazard, 0, wxALL, 5 );

	checkBusiness = new wxCheckBox( scrolledWindow, wxID_ANY, _("Business"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkBusiness, 0, wxALL, 5 );

	checkBoatRamp = new wxCheckBox( scrolledWindow, wxID_ANY, _("Boat Ramp"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkBoatRamp, 0, wxALL, 5 );

	checkBridge = new wxCheckBox( scrolledWindow, wxID_ANY, _("Bridge"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkBridge, 0, wxALL, 5 );

	checkDam = new wxCheckBox( scrolledWindow, wxID_ANY, _("Dam"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkDam, 0, wxALL, 5 );

	checkFerry = new wxCheckBox( scrolledWindow, wxID_ANY, _("Ferry"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkFerry, 0, wxALL, 5 );

	checkLock = new wxCheckBox( scrolledWindow, wxID_ANY, _("Lock"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerScrollWindow->Add( checkLock, 0, wxALL, 5 );


	scrolledWindow->SetSizer( sizerScrollWindow );
	scrolledWindow->Layout();
	sizerScrollWindow->Fit( scrolledWindow );
	SizerCheckBoxes->Add( scrolledWindow, 1, wxEXPAND | wxALL, 5 );


	sizerPanel->Add( SizerCheckBoxes, 1, wxEXPAND, 5 );

	wxBoxSizer* sizerDisclaimer;
	sizerDisclaimer = new wxBoxSizer( wxHORIZONTAL );

	bitmapGarmin = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizerDisclaimer->Add( bitmapGarmin, 0, wxALL, 5 );

	wxBoxSizer* sizerLabels;
	sizerLabels = new wxBoxSizer( wxVERTICAL );

	labelDisclaimer = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	labelDisclaimer->Wrap( -1 );
	sizerLabels->Add( labelDisclaimer, 0, wxALL, 5 );


	sizerDisclaimer->Add( sizerLabels, 1, wxEXPAND, 5 );


	sizerPanel->Add( sizerDisclaimer, 0, wxEXPAND, 5 );


	this->SetSizer( sizerPanel );
	this->Layout();
}

DemoToolboxBase::~DemoToolboxBase()
{
}

DemoActiveCaptainBase::DemoActiveCaptainBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* sizerDialog;
	sizerDialog = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerPOI;
	sizerPOI = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizerInfo;
	sizerInfo = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* sizerLabels;
	sizerLabels = new wxBoxSizer( wxVERTICAL );

	labelID = new wxStaticText( this, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	labelID->Wrap( -1 );
	sizerLabels->Add( labelID, 0, wxALL|wxEXPAND, 5 );

	labelName = new wxStaticText( this, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	labelName->Wrap( -1 );
	sizerLabels->Add( labelName, 0, wxALL|wxEXPAND, 5 );

	labelLatitude = new wxStaticText( this, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	labelLatitude->Wrap( -1 );
	sizerLabels->Add( labelLatitude, 0, wxALL|wxEXPAND, 5 );

	labelLongitude = new wxStaticText( this, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	labelLongitude->Wrap( -1 );
	sizerLabels->Add( labelLongitude, 0, wxALL, 5 );


	sizerInfo->Add( sizerLabels, 1, wxEXPAND, 5 );

	wxBoxSizer* sizerIcon;
	sizerIcon = new wxBoxSizer( wxVERTICAL );

	bitmapType = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizerIcon->Add( bitmapType, 0, wxALL, 5 );


	sizerInfo->Add( sizerIcon, 1, wxEXPAND, 5 );


	sizerPOI->Add( sizerInfo, 1, wxEXPAND, 5 );

	labelInformation = new wxStaticText( this, wxID_ANY, _("Information"), wxDefaultPosition, wxDefaultSize, 0 );
	labelInformation->Wrap( -1 );
	sizerPOI->Add( labelInformation, 0, wxALL, 5 );

	textInformation = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	sizerPOI->Add( textInformation, 2, wxALL|wxEXPAND, 5 );


	sizerDialog->Add( sizerPOI, 2, wxEXPAND, 5 );

	wxBoxSizer* sizerButtons;
	sizerButtons = new wxBoxSizer( wxHORIZONTAL );


	sizerButtons->Add( 0, 0, 1, wxEXPAND, 5 );

	buttonOK = new wxButton( this, wxID_ANY, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerButtons->Add( buttonOK, 0, wxALL, 5 );


	sizerDialog->Add( sizerButtons, 0, wxEXPAND, 5 );


	this->SetSizer( sizerDialog );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DemoActiveCaptainBase::OnInit ) );
	buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DemoActiveCaptainBase::OnOK ), NULL, this );
}

DemoActiveCaptainBase::~DemoActiveCaptainBase()
{
}
