///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MainWindow.h"

#include "embedded_files/circle.png.h"
#include "embedded_files/civitai.png.h"
#include "embedded_files/controlnet.png.h"
#include "embedded_files/cross_circle.png.h"
#include "embedded_files/cube.png.h"
#include "embedded_files/dice_four.png.h"
#include "embedded_files/disk.png.h"
#include "embedded_files/drag.png.h"
#include "embedded_files/file_import.png.h"
#include "embedded_files/forward.png.h"
#include "embedded_files/images.png.h"
#include "embedded_files/interrogation.png.h"
#include "embedded_files/palette.png.h"
#include "embedded_files/pause.png.h"
#include "embedded_files/picture.png.h"
#include "embedded_files/play.png.h"
#include "embedded_files/preview.png.h"
#include "embedded_files/refresh.png.h"
#include "embedded_files/replace.png.h"
#include "embedded_files/sd.cpp.gui_blankimage.png.h"
#include "embedded_files/settings.png.h"
#include "embedded_files/square.png.h"
#include "embedded_files/trash.png.h"
#include "embedded_files/triangle.png.h"

// Using the construction of a static object to ensure that the help provider is set
// wx Manages the most recent HelpProvider passed to ::Set, but not the previous ones
// If ::Set gets called more than once, the previous one is returned and should be deleted
class wxFBContextSensitiveHelpSetter
{
public:
wxFBContextSensitiveHelpSetter()
{
wxSimpleHelpProvider* help = new wxSimpleHelpProvider();
wxHelpProvider* old = wxHelpProvider::Set( help );
if (old != 0){
delete old;
}
}
};

static wxFBContextSensitiveHelpSetter s_wxFBSetTheHelpProvider;
///////////////////////////////////////////////////////////////////////////

mainUI::mainUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxFrame( parent, id, title, pos, size, style, name )
{
	this->SetSizeHints( wxSize( 1024,768 ), wxSize( 3072,4096 ) );

	sizer0001 = new wxBoxSizer( wxVERTICAL );

	m_panel10 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizer0021;
	sizer0021 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer97;
	bSizer97 = new wxBoxSizer( wxHORIZONTAL );

	m_settings = new wxBitmapButton( m_panel10, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 32,-1 ), wxBU_AUTODRAW|0 );

	m_settings->SetBitmap( settings_png_to_wx_bitmap() );
	bSizer97->Add( m_settings, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_refrersh = new wxBitmapButton( m_panel10, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 32,-1 ), wxBU_AUTODRAW|0 );

	m_refrersh->SetBitmap( refresh_png_to_wx_bitmap() );
	bSizer97->Add( m_refrersh, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_about = new wxBitmapButton( m_panel10, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 32,-1 ), wxBU_AUTODRAW|0 );

	m_about->SetBitmap( cube_png_to_wx_bitmap() );
	m_about->SetToolTip( _("About the sd.cpp.gui.wx") );
	m_about->SetHelpText( _("About the program") );

	bSizer97->Add( m_about, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_civitai = new wxBitmapButton( m_panel10, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 32,-1 ), wxBU_AUTODRAW|0 );

	m_civitai->SetBitmap( civitai_png_to_wx_bitmap() );
	m_civitai->SetToolTip( _("Open CivitAi.com model browser") );
	m_civitai->SetHelpText( _("With CivitAi.com model browser, you can search for models or download models") );

	bSizer97->Add( m_civitai, 0, wxALL|wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALIGN_CENTER_VERTICAL, 5 );

	m_showWidget = new wxBitmapToggleButton( m_panel10, wxID_ANY, images_png_to_wx_bitmap(), wxDefaultPosition, wxSize( 32,-1 ), 0 );

	m_showWidget->SetBitmap( images_png_to_wx_bitmap() );
	m_showWidget->SetToolTip( _("Show Widget") );

	bSizer97->Add( m_showWidget, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_stop_background_process = new wxBitmapButton( m_panel10, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 32,-1 ), wxBU_AUTODRAW|0 );

	m_stop_background_process->SetBitmap( cross_circle_png_to_wx_bitmap() );
	m_stop_background_process->Enable( false );
	m_stop_background_process->Hide();
	m_stop_background_process->SetToolTip( _("Stop and restart the background process") );
	m_stop_background_process->SetHelpText( _("This will send a terminate to the background diffuser process. This will clear up all used memory (eg. freeing up all the loaded models)") );

	bSizer97->Add( m_stop_background_process, 0, wxALL|wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticline5 = new wxStaticLine( m_panel10, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer97->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );


	sizer0021->Add( bSizer97, 0, 0, 5 );

	wxBoxSizer* bSizer98;
	bSizer98 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText160 = new wxStaticText( m_panel10, wxID_ANY, _("Model:"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_staticText160->Wrap( 0 );
	bSizer98->Add( m_staticText160, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_modelChoices;
	m_model = new wxChoice( m_panel10, wxID_ANY, wxDefaultPosition, wxSize( 200,-1 ), m_modelChoices, 0 );
	m_model->SetSelection( 0 );
	m_model->Enable( false );

	bSizer98->Add( m_model, 0, wxALL|wxEXPAND, 5 );


	sizer0021->Add( bSizer98, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer100;
	bSizer100 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText72 = new wxStaticText( m_panel10, wxID_ANY, _("Type:"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_staticText72->Wrap( -1 );
	bSizer100->Add( m_staticText72, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_typeChoices;
	m_type = new wxChoice( m_panel10, wxID_ANY, wxDefaultPosition, wxSize( 200,-1 ), m_typeChoices, 0 );
	m_type->SetSelection( 0 );
	m_type->SetToolTip( _("Weight type. If not specified, the default is the type of the weight file.") );

	bSizer100->Add( m_type, 0, wxALL|wxEXPAND, 5 );


	sizer0021->Add( bSizer100, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer120;
	bSizer120 = new wxBoxSizer( wxVERTICAL );

	m_currentStatus = new wxStaticText( m_panel10, wxID_ANY, _("Current job: none"), wxDefaultPosition, wxDefaultSize, 0 );
	m_currentStatus->Wrap( -1 );
	m_currentStatus->SetMinSize( wxSize( 300,-1 ) );

	bSizer120->Add( m_currentStatus, 0, wxLEFT|wxTOP, 5 );

	m_currentProgress = new wxGauge( m_panel10, wxID_ANY, 100, wxDefaultPosition, wxSize( 300,-1 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
	m_currentProgress->SetValue( 0 );
	bSizer120->Add( m_currentProgress, 0, wxLEFT, 5 );


	sizer0021->Add( bSizer120, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer123;
	bSizer123 = new wxBoxSizer( wxVERTICAL );


	sizer0021->Add( bSizer123, 1, wxEXPAND, 5 );


	m_panel10->SetSizer( sizer0021 );
	m_panel10->Layout();
	sizer0021->Fit( m_panel10 );
	sizer0001->Add( m_panel10, 0, wxEXPAND, 5 );

	m_panel31 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE|wxTAB_TRAVERSAL );
	bSizer138 = new wxBoxSizer( wxHORIZONTAL );

	m_notebook1302 = new wxNotebook( m_panel31, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxBK_DEFAULT|wxBORDER_THEME );
	m_notebook1302->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	m_notebook1302->SetMinSize( wxSize( 400,200 ) );

	m_jobs_panel = new wxPanel( m_notebook1302, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("jobs") );
	m_jobs_panel->SetMinSize( wxSize( 300,-1 ) );

	wxBoxSizer* bSizer104;
	bSizer104 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );

	m_start_jobs = new wxButton( m_jobs_panel, wxID_ANY, _("Resume all"), wxDefaultPosition, wxDefaultSize, 0 );

	m_start_jobs->SetBitmap( forward_png_to_wx_bitmap() );
	bSizer18->Add( m_start_jobs, 0, wxALL, 5 );

	m_pause_jobs = new wxButton( m_jobs_panel, wxID_ANY, _("Pause all"), wxDefaultPosition, wxDefaultSize, 0 );

	m_pause_jobs->SetBitmap( pause_png_to_wx_bitmap() );
	bSizer18->Add( m_pause_jobs, 0, wxALL, 5 );

	m_delete_all_jobs = new wxButton( m_jobs_panel, wxID_ANY, _("Delete all"), wxDefaultPosition, wxDefaultSize, 0 );

	m_delete_all_jobs->SetBitmap( trash_png_to_wx_bitmap() );
	m_delete_all_jobs->Enable( false );

	bSizer18->Add( m_delete_all_jobs, 0, wxALL, 5 );

	m_static_number_of_jobs = new wxStaticText( m_jobs_panel, wxID_ANY, _("Number of jobs: 0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_number_of_jobs->Wrap( -1 );
	bSizer18->Add( m_static_number_of_jobs, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer17->Add( bSizer18, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer78;
	bSizer78 = new wxBoxSizer( wxHORIZONTAL );

	m_splitter2 = new wxSplitterWindow( m_jobs_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter2->SetSashGravity( 1 );
	m_splitter2->Connect( wxEVT_IDLE, wxIdleEventHandler( mainUI::m_splitter2OnIdle ), NULL, this );
	m_splitter2->SetMinimumPaneSize( 200 );

	m_panel14 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel14->SetMinSize( wxSize( 300,-1 ) );

	wxBoxSizer* bSizer79;
	bSizer79 = new wxBoxSizer( wxVERTICAL );

	m_joblist = new wxDataViewListCtrl( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES|wxDV_MULTIPLE|wxDV_ROW_LINES|wxDV_VERT_RULES );
	m_joblist->SetExtraStyle( wxWS_EX_VALIDATE_RECURSIVELY );

	m_dataViewListColumn32 = m_joblist->AppendTextColumn( _("Id"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_HIDDEN|wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	m_dataViewListColumn321 = m_joblist->AppendTextColumn( _("Crated at"), wxDATAVIEW_CELL_INERT, 120, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	m_dataViewListColumn3211 = m_joblist->AppendTextColumn( _("Type"), wxDATAVIEW_CELL_INERT, 100, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn32111 = m_joblist->AppendTextColumn( _("Model"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn321111 = m_joblist->AppendTextColumn( _("Sampler"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn3211111 = m_joblist->AppendTextColumn( _("Seed"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn32111111 = m_joblist->AppendProgressColumn( _("Progress"), wxDATAVIEW_CELL_INERT, 80, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn321111111 = m_joblist->AppendTextColumn( _("Speed"), wxDATAVIEW_CELL_INERT, 110, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn3211111111 = m_joblist->AppendTextColumn( _("Status"), wxDATAVIEW_CELL_ACTIVATABLE, 150, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn32111111111 = m_joblist->AppendTextColumn( wxEmptyString, wxDATAVIEW_CELL_ACTIVATABLE, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	bSizer79->Add( m_joblist, 1, wxEXPAND, 5 );


	m_panel14->SetSizer( bSizer79 );
	m_panel14->Layout();
	bSizer79->Fit( m_panel14 );
	m_splitter2->Initialize( m_panel14 );
	bSizer78->Add( m_splitter2, 1, wxEXPAND, 5 );


	bSizer17->Add( bSizer78, 2, wxEXPAND, 5 );

	wxBoxSizer* bSizer9011;
	bSizer9011 = new wxBoxSizer( wxVERTICAL );

	m_scrolledWindow41 = new wxScrolledWindow( m_jobs_panel, wxID_ANY, wxDefaultPosition, wxSize( -1,280 ), wxBORDER_THEME|wxHSCROLL );
	m_scrolledWindow41->SetScrollRate( 5, 5 );
	bSizer8911 = new wxBoxSizer( wxHORIZONTAL );

	m_bitmap6 = new wxStaticBitmap( m_scrolledWindow41, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8911->Add( m_bitmap6, 0, wxALL, 5 );


	m_scrolledWindow41->SetSizer( bSizer8911 );
	m_scrolledWindow41->Layout();
	bSizer9011->Add( m_scrolledWindow41, 1, wxEXPAND|wxALL, 5 );


	bSizer17->Add( bSizer9011, 1, wxEXPAND, 5 );


	bSizer104->Add( bSizer17, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer106;
	bSizer106 = new wxBoxSizer( wxVERTICAL );

	m_staticText64 = new wxStaticText( m_jobs_panel, wxID_ANY, _("Job details"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	m_staticText64->Wrap( -1 );
	bSizer106->Add( m_staticText64, 0, wxALL|wxEXPAND, 5 );

	m_joblist_item_details = new wxDataViewListCtrl( m_jobs_panel, wxID_ANY, wxDefaultPosition, wxSize( 480,-1 ), wxDV_NO_HEADER|wxDV_ROW_LINES|wxDV_SINGLE|wxDV_VARIABLE_LINE_HEIGHT|wxDV_VERT_RULES|wxBORDER_DEFAULT );
	m_dataViewListColumn1 = m_joblist_item_details->AppendTextColumn( wxEmptyString, wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn2 = m_joblist_item_details->AppendTextColumn( wxEmptyString, wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn2->GetRenderer()->EnableEllipsize( wxELLIPSIZE_NONE );
	bSizer106->Add( m_joblist_item_details, 1, wxALL|wxEXPAND, 5 );


	bSizer104->Add( bSizer106, 0, wxEXPAND, 5 );


	m_jobs_panel->SetSizer( bSizer104 );
	m_jobs_panel->Layout();
	bSizer104->Fit( m_jobs_panel );
	m_notebook1302->AddPage( m_jobs_panel, _("Jobs and Images"), true );
	m_text2img_panel = new wxPanel( m_notebook1302, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizer0004;
	sizer0004 = new wxBoxSizer( wxVERTICAL );

	sizer0004->SetMinSize( wxSize( -1,100 ) );
	wxBoxSizer* bSizer811;
	bSizer811 = new wxBoxSizer( wxHORIZONTAL );

	bSizer811->SetMinSize( wxSize( 300,-1 ) );
	wxBoxSizer* bSizer331;
	bSizer331 = new wxBoxSizer( wxVERTICAL );

	bSizer331->SetMinSize( wxSize( -1,100 ) );
	m_notebook3 = new wxNotebook( m_text2img_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_RIGHT );
	m_diffusionPreviewTab = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_diffusionPreviewTab->Enable( false );

	wxBoxSizer* bSizer991;
	bSizer991 = new wxBoxSizer( wxVERTICAL );

	m_txt2txtDeiffusionPreview = new wxStaticBitmap( m_diffusionPreviewTab, wxID_ANY, preview_png_to_wx_bitmap(), wxDefaultPosition, wxSize( -1,-1 ), wxFULL_REPAINT_ON_RESIZE );
	m_txt2txtDeiffusionPreview->SetBackgroundColour( wxColour( 64, 64, 64 ) );
	m_txt2txtDeiffusionPreview->SetMinSize( wxSize( 200,-1 ) );

	bSizer991->Add( m_txt2txtDeiffusionPreview, 1, wxEXPAND, 5 );


	m_diffusionPreviewTab->SetSizer( bSizer991 );
	m_diffusionPreviewTab->Layout();
	bSizer991->Fit( m_diffusionPreviewTab );
	m_notebook3->AddPage( m_diffusionPreviewTab, _("Diffusion"), false );
	m_controlnetPreviewTab = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer981;
	bSizer981 = new wxBoxSizer( wxVERTICAL );

	m_controlnetImagePreview = new wxStaticBitmap( m_controlnetPreviewTab, wxID_ANY, controlnet_png_to_wx_bitmap(), wxDefaultPosition, wxSize( -1,-1 ), wxFULL_REPAINT_ON_RESIZE );
	m_controlnetImagePreview->SetBackgroundColour( wxColour( 64, 64, 64 ) );
	m_controlnetImagePreview->SetMinSize( wxSize( 200,-1 ) );

	bSizer981->Add( m_controlnetImagePreview, 1, wxEXPAND, 5 );


	m_controlnetPreviewTab->SetSizer( bSizer981 );
	m_controlnetPreviewTab->Layout();
	bSizer981->Fit( m_controlnetPreviewTab );
	m_notebook3->AddPage( m_controlnetPreviewTab, _("Controlnet"), true );
	m_openPoseWindow = new wxScrolledWindow( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_openPoseWindow->SetScrollRate( 5, 5 );
	m_openPoseWindow->Hide();

	m_notebook3->AddPage( m_openPoseWindow, _("OpenPose Editor"), false );

	bSizer331->Add( m_notebook3, 1, wxEXPAND, 5 );


	bSizer811->Add( bSizer331, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer82;
	bSizer82 = new wxBoxSizer( wxVERTICAL );

	bSizer82->SetMinSize( wxSize( 300,-1 ) );
	m_prompt = new wxTextCtrl( m_text2img_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_RICH|wxTE_RICH2|wxTE_WORDWRAP );
	m_prompt->DragAcceptFiles( true );
	m_prompt->SetMinSize( wxSize( 100,100 ) );

	bSizer82->Add( m_prompt, 1, wxALL|wxEXPAND, 1 );

	m_neg_prompt = new wxTextCtrl( m_text2img_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_RICH|wxTE_RICH2|wxTE_WORDWRAP );
	m_neg_prompt->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	m_neg_prompt->DragAcceptFiles( true );
	m_neg_prompt->SetMinSize( wxSize( 100,100 ) );

	bSizer82->Add( m_neg_prompt, 1, wxALL|wxEXPAND, 1 );


	bSizer82->Add( 0, 0, 1, wxEXPAND, 5 );


	bSizer811->Add( bSizer82, 1, wxEXPAND, 5 );


	sizer0004->Add( bSizer811, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxHORIZONTAL );

	m_controlnetImageOpen = new wxFilePickerCtrl( m_text2img_panel, wxID_ANY, wxEmptyString, _("Select a file"), _("Image files (*.jpg;*.jpeg;*.png;*.JPG;*.JPEG;*.PNG)|*.jpg;*.jpeg;*.png;*.JPG;*.JPEG;*.PNG"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN );
	m_controlnetImageOpen->SetMinSize( wxSize( 260,-1 ) );

	bSizer34->Add( m_controlnetImageOpen, 0, wxALL, 5 );

	cnOnCpu = new wxCheckBox( m_text2img_panel, wxID_ANY, _("CN on CPU"), wxDefaultPosition, wxDefaultSize, 0 );
	cnOnCpu->SetToolTip( _("Keep controlnet on CPU") );

	bSizer34->Add( cnOnCpu, 0, wxALL|wxEXPAND, 5 );

	wxArrayString m_controlnetModelsChoices;
	m_controlnetModels = new wxChoice( m_text2img_panel, wxID_ANY, wxDefaultPosition, wxSize( 210,-1 ), m_controlnetModelsChoices, 0 );
	m_controlnetModels->SetSelection( 0 );
	m_controlnetModels->Enable( false );

	bSizer34->Add( m_controlnetModels, 0, wxALL, 5 );

	m_controlnetStrength = new wxSpinCtrlDouble( m_text2img_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 142,-1 ), wxSP_ARROW_KEYS, 0, 0.9, 0.9, 0.1 );
	m_controlnetStrength->SetDigits( 1 );
	bSizer34->Add( m_controlnetStrength, 0, wxALL, 5 );

	m_controlnetImagePreviewButton = new wxButton( m_text2img_panel, wxID_ANY, _("Show full"), wxDefaultPosition, wxDefaultSize, 0 );
	m_controlnetImagePreviewButton->Enable( false );
	m_controlnetImagePreviewButton->SetToolTip( _("Show the original controlnet image") );

	bSizer34->Add( m_controlnetImagePreviewButton, 0, wxALL, 5 );

	m_controlnetImageDelete = new wxBitmapButton( m_text2img_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_controlnetImageDelete->SetBitmap( trash_png_to_wx_bitmap() );
	m_controlnetImageDelete->Enable( false );
	m_controlnetImageDelete->SetToolTip( _("Remove control image") );

	bSizer34->Add( m_controlnetImageDelete, 0, wxALL, 5 );


	sizer0004->Add( bSizer34, 0, wxEXPAND, 5 );


	m_text2img_panel->SetSizer( sizer0004 );
	m_text2img_panel->Layout();
	sizer0004->Fit( m_text2img_panel );
	m_notebook1302->AddPage( m_text2img_panel, _("txt2img"), false );
	m_image2image_panel = new wxPanel( m_notebook1302, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("img2img") );
	m_image2image_panel->DragAcceptFiles( true );
	m_image2image_panel->SetMinSize( wxSize( 300,-1 ) );

	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxVERTICAL );

	bSizer29->SetMinSize( wxSize( 300,-1 ) );
	m_notebook4 = new wxNotebook( m_image2image_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_RIGHT );
	m_panel23 = new wxPanel( m_notebook4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel23->Enable( false );

	wxBoxSizer* bSizer1003;
	bSizer1003 = new wxBoxSizer( wxVERTICAL );

	m_img2imgDiffusionPreview = new wxStaticBitmap( m_panel23, wxID_ANY, preview_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	m_img2imgDiffusionPreview->SetBackgroundColour( wxColour( 64, 64, 64 ) );
	m_img2imgDiffusionPreview->DragAcceptFiles( true );
	m_img2imgDiffusionPreview->SetMinSize( wxSize( 200,-1 ) );

	bSizer1003->Add( m_img2imgDiffusionPreview, 1, wxEXPAND, 5 );


	m_panel23->SetSizer( bSizer1003 );
	m_panel23->Layout();
	bSizer1003->Fit( m_panel23 );
	m_notebook4->AddPage( m_panel23, _("Diffusion"), false );
	m_panel25 = new wxPanel( m_notebook4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer1026;
	bSizer1026 = new wxBoxSizer( wxVERTICAL );

	m_panel26 = new wxPanel( m_panel25, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer1028;
	bSizer1028 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer1030;
	bSizer1030 = new wxBoxSizer( wxHORIZONTAL );

	m_inpaintBrushSize = new wxStaticText( m_panel26, wxID_ANY, _("Brush size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_inpaintBrushSize->Wrap( -1 );
	m_inpaintBrushSize->SetMinSize( wxSize( 100,-1 ) );

	bSizer1030->Add( m_inpaintBrushSize, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_inpaintBrushSizeSlider = new wxSlider( m_panel26, wxID_ANY, 10, 1, 40, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_VALUE_LABEL );
	bSizer1030->Add( m_inpaintBrushSizeSlider, 0, wxALL, 5 );

	m_inpaintZoom = new wxStaticText( m_panel26, wxID_ANY, _("Zoom: 1x"), wxDefaultPosition, wxDefaultSize, 0 );
	m_inpaintZoom->Wrap( -1 );
	m_inpaintZoom->SetMinSize( wxSize( 100,-1 ) );

	bSizer1030->Add( m_inpaintZoom, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_inpaintImageResolution = new wxStaticText( m_panel26, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_inpaintImageResolution->Wrap( -1 );
	m_inpaintImageResolution->SetMinSize( wxSize( 150,-1 ) );

	bSizer1030->Add( m_inpaintImageResolution, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_inpaintSaveMask = new wxBitmapButton( m_panel26, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 28,28 ), wxBU_AUTODRAW|0 );

	m_inpaintSaveMask->SetBitmap( disk_png_to_wx_bitmap() );
	m_inpaintSaveMask->SetToolTip( _("Save the mask into png") );

	bSizer1030->Add( m_inpaintSaveMask, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_inpaintInvert = new wxBitmapButton( m_panel26, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 28,28 ), wxBU_AUTODRAW|0 );

	m_inpaintInvert->SetBitmap( replace_png_to_wx_bitmap() );
	m_inpaintInvert->SetToolTip( _("Invert mask") );

	bSizer1030->Add( m_inpaintInvert, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_inpaintResizeToSdSize = new wxBitmapButton( m_panel26, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 28,28 ), wxBU_AUTODRAW|0 );

	m_inpaintResizeToSdSize->SetBitmap( drag_png_to_wx_bitmap() );
	m_inpaintResizeToSdSize->SetToolTip( _("Resize the image to fit into the diffusion's width and height.\nWARNING: this will delete the current mask if have!") );

	bSizer1030->Add( m_inpaintResizeToSdSize, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_inpaintClearMask = new wxBitmapButton( m_panel26, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 28,28 ), wxBU_AUTODRAW|0 );

	m_inpaintClearMask->SetBitmap( trash_png_to_wx_bitmap() );
	m_inpaintClearMask->Enable( false );
	m_inpaintClearMask->SetToolTip( _("Clear the mask") );

	bSizer1030->Add( m_inpaintClearMask, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_inpaintOpenMask = new wxFilePickerCtrl( m_panel26, wxID_ANY, wxEmptyString, _("Select a file"), _("Image files (*.jpg;*.jpeg;*.png;*.JPG;*.JPEG;*.PNG)|*.jpg;*.jpeg;*.png;*.JPG;*.JPEG;*.PNG"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_SMALL );
	m_inpaintOpenMask->SetToolTip( _("Open a mask image from file") );

	bSizer1030->Add( m_inpaintOpenMask, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer1028->Add( bSizer1030, 0, 0, 0 );

	wxBoxSizer* bSizer1029;
	bSizer1029 = new wxBoxSizer( wxHORIZONTAL );

	m_inPaintBrushStyleCircle = new wxBitmapToggleButton( m_panel26, wxID_ANY, circle_png_to_wx_bitmap(), wxDefaultPosition, wxSize( 28,28 ), 0 );
	m_inPaintBrushStyleCircle->SetValue( true );

	m_inPaintBrushStyleCircle->SetBitmap( circle_png_to_wx_bitmap() );
	m_inPaintBrushStyleCircle->SetToolTip( _("Change the brush shape to circle") );

	bSizer1029->Add( m_inPaintBrushStyleCircle, 0, wxALL, 5 );

	m_inPaintBrushStyleSquare = new wxBitmapToggleButton( m_panel26, wxID_ANY, square_png_to_wx_bitmap(), wxDefaultPosition, wxSize( 28,28 ), 0 );

	m_inPaintBrushStyleSquare->SetBitmap( square_png_to_wx_bitmap() );
	m_inPaintBrushStyleSquare->SetToolTip( _("Change the brush shape to square") );

	bSizer1029->Add( m_inPaintBrushStyleSquare, 0, wxALL, 5 );

	m_inPaintBrushStyleTriangle = new wxBitmapToggleButton( m_panel26, wxID_ANY, triangle_png_to_wx_bitmap(), wxDefaultPosition, wxSize( 28,28 ), 0 );

	m_inPaintBrushStyleTriangle->SetBitmap( triangle_png_to_wx_bitmap() );
	m_inPaintBrushStyleTriangle->SetToolTip( _("Change the brush shape to triangle") );

	bSizer1029->Add( m_inPaintBrushStyleTriangle, 0, wxALL, 5 );


	bSizer1029->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText69 = new wxStaticText( m_panel26, wxID_ANY, _("Ctrl + Mouse wheel to ZOOM\nShift + Mouse wheel to Change brush size"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText69->Wrap( -1 );
	bSizer1029->Add( m_staticText69, 1, wxALIGN_CENTER_VERTICAL, 0 );


	bSizer1028->Add( bSizer1029, 0, 0, 0 );


	m_panel26->SetSizer( bSizer1028 );
	m_panel26->Layout();
	bSizer1028->Fit( m_panel26 );
	bSizer1026->Add( m_panel26, 0, wxALL, 5 );

	m_img2imPanel = new wxScrolledWindow( m_panel25, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxHSCROLL|wxVSCROLL, wxT("inpaintEditor") );
	m_img2imPanel->SetScrollRate( 5, 5 );
	m_img2imPanel->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );
	m_img2imPanel->DragAcceptFiles( true );
	m_img2imPanel->SetMinSize( wxSize( 256,256 ) );

	bSizer1026->Add( m_img2imPanel, 1, wxALL|wxEXPAND, 5 );

	m_panel231 = new wxPanel( m_panel25, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer1027;
	bSizer1027 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText691 = new wxStaticText( m_panel231, wxID_ANY, _("Outpaint Top:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText691->Wrap( -1 );
	bSizer1027->Add( m_staticText691, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_inpaintCanvasTop = new wxTextCtrl( m_panel231, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT );
	bSizer1027->Add( m_inpaintCanvasTop, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText701 = new wxStaticText( m_panel231, wxID_ANY, _("Right:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText701->Wrap( -1 );
	bSizer1027->Add( m_staticText701, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_inpaintCanvasRight = new wxTextCtrl( m_panel231, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT );
	bSizer1027->Add( m_inpaintCanvasRight, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText71 = new wxStaticText( m_panel231, wxID_ANY, _("Bottom:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText71->Wrap( -1 );
	bSizer1027->Add( m_staticText71, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_inpaintCanvasBottom = new wxTextCtrl( m_panel231, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT );
	bSizer1027->Add( m_inpaintCanvasBottom, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText721 = new wxStaticText( m_panel231, wxID_ANY, _("Left:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText721->Wrap( -1 );
	bSizer1027->Add( m_staticText721, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_inpaintCanvasLeft = new wxTextCtrl( m_panel231, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT );
	bSizer1027->Add( m_inpaintCanvasLeft, 0, wxALL, 5 );

	m_inpaintCanvasResizeApply = new wxButton( m_panel231, wxID_ANY, _("Resize"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1027->Add( m_inpaintCanvasResizeApply, 0, wxALL, 5 );


	m_panel231->SetSizer( bSizer1027 );
	m_panel231->Layout();
	bSizer1027->Fit( m_panel231 );
	bSizer1026->Add( m_panel231, 0, wxALL, 5 );


	m_panel25->SetSizer( bSizer1026 );
	m_panel25->Layout();
	bSizer1026->Fit( m_panel25 );
	m_notebook4->AddPage( m_panel25, _("Preview and InPaint"), true );

	bSizer29->Add( m_notebook4, 0, 0, 5 );


	bSizer28->Add( bSizer29, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );

	bSizer33->SetMinSize( wxSize( 300,-1 ) );
	m_prompt2 = new wxTextCtrl( m_image2image_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_RICH|wxTE_RICH2|wxTE_WORDWRAP );
	m_prompt2->SetMinSize( wxSize( 100,100 ) );

	bSizer33->Add( m_prompt2, 1, wxEXPAND|wxALL, 1 );

	m_neg_prompt2 = new wxTextCtrl( m_image2image_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_RICH|wxTE_RICH2|wxTE_WORDWRAP );
	m_neg_prompt2->SetMinSize( wxSize( 100,100 ) );

	bSizer33->Add( m_neg_prompt2, 1, wxEXPAND|wxALL, 1 );


	bSizer33->Add( 0, 0, 1, wxEXPAND, 5 );

	m_img2imgOpen = new wxFilePickerCtrl( m_image2image_panel, wxID_ANY, wxEmptyString, _("Select an image"), _("Image files (*.jpg;*.jpeg;*.png;*.JPG;*.JPEG;*.PNG)|*.jpg;*.jpeg;*.png;*.JPG;*.JPEG;*.PNG"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	m_img2imgOpen->SetMinSize( wxSize( 470,-1 ) );

	bSizer33->Add( m_img2imgOpen, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText24 = new wxStaticText( m_image2image_panel, wxID_ANY, _("Strength:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	bSizer33->Add( m_staticText24, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_strength = new wxSpinCtrlDouble( m_image2image_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1, 0.75, 0.1 );
	m_strength->SetDigits( 2 );
	bSizer33->Add( m_strength, 0, wxALL, 5 );

	m_img2im_preview_img = new wxButton( m_image2image_panel, wxID_ANY, _("Show full"), wxDefaultPosition, wxDefaultSize, 0 );
	m_img2im_preview_img->Enable( false );

	bSizer33->Add( m_img2im_preview_img, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_delete_initial_img = new wxBitmapButton( m_image2image_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_delete_initial_img->SetBitmap( trash_png_to_wx_bitmap() );
	bSizer33->Add( m_delete_initial_img, 0, wxALL, 5 );


	bSizer28->Add( bSizer33, 1, wxEXPAND, 5 );


	m_image2image_panel->SetSizer( bSizer28 );
	m_image2image_panel->Layout();
	bSizer28->Fit( m_image2image_panel );
	m_notebook1302->AddPage( m_image2image_panel, _("img2img"), false );
	m_upscaler = new wxPanel( m_notebook1302, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("upscaler") );
	m_upscaler->DragAcceptFiles( true );
	m_upscaler->SetMinSize( wxSize( 300,-1 ) );

	wxBoxSizer* bSizer68;
	bSizer68 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer999;
	bSizer999 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer1000;
	bSizer1000 = new wxBoxSizer( wxHORIZONTAL );

	m_upscaler_source_image = new wxStaticBitmap( m_upscaler, wxID_ANY, sd_cpp_gui_blankimage_png_to_wx_bitmap(), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_upscaler_source_image->SetBackgroundColour( wxColour( 64, 64, 64 ) );
	m_upscaler_source_image->SetMinSize( wxSize( 200,-1 ) );

	bSizer1000->Add( m_upscaler_source_image, 1, wxEXPAND, 5 );


	bSizer999->Add( bSizer1000, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer69;
	bSizer69 = new wxBoxSizer( wxVERTICAL );

	bSizer69->SetMinSize( wxSize( 300,-1 ) );
	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText62 = new wxStaticText( m_upscaler, wxID_ANY, _("Source Image"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_staticText62->Wrap( -1 );
	bSizer71->Add( m_staticText62, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_upscaler_filepicker = new wxFilePickerCtrl( m_upscaler, wxID_ANY, wxEmptyString, _("Select an image"), _("Image files (*.jpg;*.jpeg;*.png;*.JPG;*.JPEG;*.PNG)|*.jpg;*.jpeg;*.png;*.JPG;*.JPEG;*.PNG"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	bSizer71->Add( m_upscaler_filepicker, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_delete_upscale_image = new wxBitmapButton( m_upscaler, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_delete_upscale_image->SetBitmap( trash_png_to_wx_bitmap() );
	bSizer71->Add( m_delete_upscale_image, 0, wxALL, 5 );


	bSizer69->Add( bSizer71, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer70;
	bSizer70 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText61 = new wxStaticText( m_upscaler, wxID_ANY, _("Upscaler model"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_staticText61->Wrap( -1 );
	bSizer70->Add( m_staticText61, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_upscaler_modelChoices;
	m_upscaler_model = new wxChoice( m_upscaler, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_upscaler_modelChoices, 0 );
	m_upscaler_model->SetSelection( 0 );
	bSizer70->Add( m_upscaler_model, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer69->Add( bSizer70, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer72;
	bSizer72 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText63 = new wxStaticText( m_upscaler, wxID_ANY, _("Width:"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_staticText63->Wrap( -1 );
	bSizer72->Add( m_staticText63, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_static_upscaler_width = new wxStaticText( m_upscaler, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_static_upscaler_width->Wrap( -1 );
	bSizer72->Add( m_static_upscaler_width, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText65 = new wxStaticText( m_upscaler, wxID_ANY, _("Height:"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_staticText65->Wrap( -1 );
	bSizer72->Add( m_staticText65, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_static_upscaler_height = new wxStaticText( m_upscaler, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_static_upscaler_height->Wrap( -1 );
	bSizer72->Add( m_static_upscaler_height, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer69->Add( bSizer72, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer721;
	bSizer721 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText631 = new wxStaticText( m_upscaler, wxID_ANY, _("Width:"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_staticText631->Wrap( -1 );
	bSizer721->Add( m_staticText631, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_static_upscaler_target_width = new wxStaticText( m_upscaler, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_static_upscaler_target_width->Wrap( -1 );
	bSizer721->Add( m_static_upscaler_target_width, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText651 = new wxStaticText( m_upscaler, wxID_ANY, _("Height:"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_staticText651->Wrap( -1 );
	bSizer721->Add( m_staticText651, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_static_upscaler_target_height = new wxStaticText( m_upscaler, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_static_upscaler_target_height->Wrap( -1 );
	bSizer721->Add( m_static_upscaler_target_height, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer69->Add( bSizer721, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer73;
	bSizer73 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText67 = new wxStaticText( m_upscaler, wxID_ANY, _("Upscale factor"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_staticText67->Wrap( -1 );
	bSizer73->Add( m_staticText67, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_upscaler_factor = new wxSpinCtrl( m_upscaler, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 142,-1 ), 0, 1, 4, 4 );
	m_upscaler_factor->Enable( false );

	bSizer73->Add( m_upscaler_factor, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer69->Add( bSizer73, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer951;
	bSizer951 = new wxBoxSizer( wxVERTICAL );

	m_upscalerHelp = new wxHtmlWindow( m_upscaler, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	m_upscalerHelp->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	bSizer951->Add( m_upscalerHelp, 1, wxALL|wxEXPAND, 5 );


	bSizer69->Add( bSizer951, 1, wxEXPAND, 5 );


	bSizer999->Add( bSizer69, 1, wxEXPAND, 5 );


	bSizer68->Add( bSizer999, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer77;
	bSizer77 = new wxBoxSizer( wxHORIZONTAL );

	m_keep_upscaler_in_memory = new wxCheckBox( m_upscaler, wxID_ANY, _("Keep upscale model in memory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_keep_upscaler_in_memory->SetToolTip( _("WARNING: If you check this box, the upscaler model will remain in memory for the next job, which could lead to a memory overflow or program crash (segfault).") );

	bSizer77->Add( m_keep_upscaler_in_memory, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_keep_other_models_in_memory = new wxCheckBox( m_upscaler, wxID_ANY, _("Keep checkpoints in memory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_keep_other_models_in_memory->SetToolTip( _("WARNING: If you check this box, the SD model from the previous job will remain in RAM/VRAM, which could potentially lead to a program crash (segfault).") );

	bSizer77->Add( m_keep_other_models_in_memory, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer68->Add( bSizer77, 0, wxEXPAND, 5 );


	m_upscaler->SetSizer( bSizer68 );
	m_upscaler->Layout();
	bSizer68->Fit( m_upscaler );
	m_notebook1302->AddPage( m_upscaler, _("Upscaler"), false );
	m_imageinfo = new wxPanel( m_notebook1302, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer115;
	bSizer115 = new wxBoxSizer( wxVERTICAL );

	m_splitter4 = new wxSplitterWindow( m_imageinfo, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER );
	m_splitter4->SetSashGravity( 0.5 );
	m_splitter4->Connect( wxEVT_IDLE, wxIdleEventHandler( mainUI::m_splitter4OnIdle ), NULL, this );
	m_splitter4->SetMinimumPaneSize( 200 );

	m_scrolledWindow5 = new wxScrolledWindow( m_splitter4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow5->SetScrollRate( 5, 5 );
	m_scrolledWindow5->SetBackgroundColour( wxColour( 64, 64, 64 ) );

	bSizer117 = new wxBoxSizer( wxVERTICAL );

	m_imageinfo_preview = new wxStaticBitmap( m_scrolledWindow5, wxID_ANY, sd_cpp_gui_blankimage_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	m_imageinfo_preview->DragAcceptFiles( true );
	m_imageinfo_preview->SetMinSize( wxSize( 384,384 ) );
	m_imageinfo_preview->SetMaxSize( wxSize( 1024,1024 ) );

	bSizer117->Add( m_imageinfo_preview, 1, wxEXPAND, 5 );


	m_scrolledWindow5->SetSizer( bSizer117 );
	m_scrolledWindow5->Layout();
	bSizer117->Fit( m_scrolledWindow5 );
	m_scrolledWindow7 = new wxScrolledWindow( m_splitter4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow7->SetScrollRate( 5, 5 );
	imageInfoSizer = new wxBoxSizer( wxVERTICAL );

	bSizer119 = new wxBoxSizer( wxVERTICAL );

	m_imageInfoPrompt = new wxTextCtrl( m_scrolledWindow7, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,120 ), wxTE_MULTILINE|wxTE_READONLY );
	bSizer119->Add( m_imageInfoPrompt, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer108;
	bSizer108 = new wxBoxSizer( wxHORIZONTAL );

	m_imageInfoPromptTo2txt2img = new wxButton( m_scrolledWindow7, wxID_ANY, _("Copy to txt2img prompt"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer108->Add( m_imageInfoPromptTo2txt2img, 0, wxALL, 5 );

	m_imageInfoPromptTo2img2img = new wxButton( m_scrolledWindow7, wxID_ANY, _("Copy to img2img prompt"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer108->Add( m_imageInfoPromptTo2img2img, 0, wxALL, 5 );


	bSizer119->Add( bSizer108, 0, wxEXPAND, 5 );

	m_imageInfoNegPrompt = new wxTextCtrl( m_scrolledWindow7, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,120 ), wxTE_MULTILINE|wxTE_READONLY );
	bSizer119->Add( m_imageInfoNegPrompt, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer1081;
	bSizer1081 = new wxBoxSizer( wxHORIZONTAL );

	m_imageInfoNegPromptTo2txt2img = new wxButton( m_scrolledWindow7, wxID_ANY, _("Copy to txt2img prompt"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1081->Add( m_imageInfoNegPromptTo2txt2img, 0, wxALL, 5 );

	m_imageInfoNegPromptTo2img2img = new wxButton( m_scrolledWindow7, wxID_ANY, _("Copy to img2img prompt"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1081->Add( m_imageInfoNegPromptTo2img2img, 0, wxALL, 5 );


	bSizer119->Add( bSizer1081, 0, wxEXPAND, 5 );

	m_imageInfoList = new wxTextCtrl( m_scrolledWindow7, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT|wxTE_MULTILINE|wxTE_READONLY );
	m_imageInfoList->SetMinSize( wxSize( -1,120 ) );

	bSizer119->Add( m_imageInfoList, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer112;
	bSizer112 = new wxBoxSizer( wxHORIZONTAL );

	m_ImageInfoTryFindModel = new wxButton( m_scrolledWindow7, wxID_ANY, _("Find and select model"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ImageInfoTryFindModel->SetToolTip( _("Try to find and load the model by the image parameters") );

	bSizer112->Add( m_ImageInfoTryFindModel, 0, wxALL, 5 );


	bSizer119->Add( bSizer112, 0, wxEXPAND, 5 );


	imageInfoSizer->Add( bSizer119, 1, wxEXPAND, 5 );


	m_scrolledWindow7->SetSizer( imageInfoSizer );
	m_scrolledWindow7->Layout();
	imageInfoSizer->Fit( m_scrolledWindow7 );
	m_splitter4->SplitVertically( m_scrolledWindow5, m_scrolledWindow7, 0 );
	bSizer115->Add( m_splitter4, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer116;
	bSizer116 = new wxBoxSizer( wxHORIZONTAL );

	m_imageInfoLoadTotxt = new wxButton( m_imageinfo, wxID_ANY, _("Load all to txt2img"), wxDefaultPosition, wxDefaultSize, 0 );
	m_imageInfoLoadTotxt->Enable( false );

	bSizer116->Add( m_imageInfoLoadTotxt, 0, wxALL, 5 );

	m_imageInfoLoadToimg2img = new wxButton( m_imageinfo, wxID_ANY, _("Load all to img2img"), wxDefaultPosition, wxDefaultSize, 0 );
	m_imageInfoLoadToimg2img->Enable( false );

	bSizer116->Add( m_imageInfoLoadToimg2img, 0, wxALL, 5 );

	m_imageInfoOpen = new wxFilePickerCtrl( m_imageinfo, wxID_ANY, wxEmptyString, _("Select a file"), _("Image files (*.jpg;*.jpeg;*.png;*.JPG;*.JPEG;*.PNG)|*.jpg;*.jpeg;*.png;*.JPG;*.JPEG;*.PNG"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	m_imageInfoOpen->SetMinSize( wxSize( 200,-1 ) );
	m_imageInfoOpen->SetMaxSize( wxSize( 400,-1 ) );

	bSizer116->Add( m_imageInfoOpen, 1, wxALL, 5 );

	m_cleanImageInfo = new wxBitmapButton( m_imageinfo, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_cleanImageInfo->SetBitmap( trash_png_to_wx_bitmap() );
	m_cleanImageInfo->SetToolTip( _("Clear") );

	bSizer116->Add( m_cleanImageInfo, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer115->Add( bSizer116, 0, wxEXPAND, 5 );


	m_imageinfo->SetSizer( bSizer115 );
	m_imageinfo->Layout();
	bSizer115->Fit( m_imageinfo );
	m_notebook1302->AddPage( m_imageinfo, _("Image info"), false );
	m_models_panel = new wxPanel( m_notebook1302, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("models") );
	m_models_panel->SetMinSize( wxSize( 300,-1 ) );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer102;
	bSizer102 = new wxBoxSizer( wxVERTICAL );

	m_modelTreeList = new wxTreeListCtrl( m_models_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_MULTIPLE );
	m_modelTreeList->SetMinSize( wxSize( -1,400 ) );

	m_modelTreeList->AppendColumn( _("Name"), wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE|wxCOL_SORTABLE );
	m_modelTreeList->AppendColumn( _("Size"), wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE|wxCOL_SORTABLE );
	m_modelTreeList->AppendColumn( _("Type"), wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE|wxCOL_SORTABLE );
	m_modelTreeList->AppendColumn( _("Hash"), wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE );

	bSizer102->Add( m_modelTreeList, 0, wxEXPAND | wxALL, 5 );

	m_modelDetailsImageList = new wxScrolledWindow( m_models_panel, wxID_ANY, wxDefaultPosition, wxSize( -1,280 ), wxBORDER_THEME|wxHSCROLL );
	m_modelDetailsImageList->SetScrollRate( 5, 5 );
	bSizer891 = new wxBoxSizer( wxHORIZONTAL );


	m_modelDetailsImageList->SetSizer( bSizer891 );
	m_modelDetailsImageList->Layout();
	bSizer102->Add( m_modelDetailsImageList, 1, wxALL|wxEXPAND, 5 );


	bSizer20->Add( bSizer102, 1, wxEXPAND, 5 );

	bSizer1001 = new wxBoxSizer( wxVERTICAL );

	m_staticText641 = new wxStaticText( m_models_panel, wxID_ANY, _("Model details"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	m_staticText641->Wrap( -1 );
	bSizer1001->Add( m_staticText641, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer107;
	bSizer107 = new wxBoxSizer( wxHORIZONTAL );

	m_ModelFavorite = new wxCheckBox( m_models_panel, wxID_ANY, _("[F]avorite"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ModelFavorite->Enable( false );

	bSizer107->Add( m_ModelFavorite, 0, wxALL|wxEXPAND, 5 );

	m_model_info_link = new wxHyperlinkCtrl( m_models_panel, wxID_ANY, _("wxFB Website"), wxT("http://www.wxformbuilder.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	m_model_info_link->Hide();

	bSizer107->Add( m_model_info_link, 0, wxALL|wxEXPAND, 5 );


	bSizer1001->Add( bSizer107, 0, wxEXPAND, 5 );

	m_model_details = new wxDataViewListCtrl( m_models_panel, wxID_ANY, wxDefaultPosition, wxSize( 480,-1 ), wxDV_NO_HEADER|wxDV_ROW_LINES|wxDV_SINGLE|wxDV_VARIABLE_LINE_HEIGHT|wxDV_VERT_RULES|wxBORDER_DEFAULT );
	m_dataViewListColumn11 = m_model_details->AppendTextColumn( wxEmptyString, wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn21 = m_model_details->AppendTextColumn( wxEmptyString, wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn21->GetRenderer()->EnableEllipsize( wxELLIPSIZE_NONE );
	bSizer1001->Add( m_model_details, 1, wxALL|wxEXPAND, 5 );

	m_model_details_description = new wxHtmlWindow( m_models_panel, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxHW_SCROLLBAR_AUTO|wxBORDER_DEFAULT|wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxVSCROLL );
	m_model_details_description->Hide();
	m_model_details_description->SetMinSize( wxSize( -1,300 ) );

	bSizer1001->Add( m_model_details_description, 0, wxALL|wxEXPAND|wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5 );


	bSizer20->Add( bSizer1001, 0, wxEXPAND, 5 );


	m_models_panel->SetSizer( bSizer20 );
	m_models_panel->Layout();
	bSizer20->Fit( m_models_panel );
	m_notebook1302->AddPage( m_models_panel, _("Models"), false );

	bSizer138->Add( m_notebook1302, 1, wxEXPAND, 5 );

	m_rightMainPanel = new wxScrolledWindow( m_panel31, wxID_ANY, wxDefaultPosition, wxSize( 340,-1 ), wxBORDER_THEME|wxHSCROLL|wxVSCROLL );
	m_rightMainPanel->SetScrollRate( 5, 5 );
	m_rightMainPanel->Hide();

	bSizer1108 = new wxBoxSizer( wxVERTICAL );

	m_panel24 = new wxPanel( m_rightMainPanel, wxID_ANY, wxDefaultPosition, wxSize( 300,-1 ), wxBORDER_NONE|wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );

	m_staticText161 = new wxStaticText( m_panel24, wxID_ANY, _("Vae:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText161->Wrap( 0 );
	fgSizer2->Add( m_staticText161, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer131;
	bSizer131 = new wxBoxSizer( wxHORIZONTAL );

	wxArrayString m_vaeChoices;
	m_vae = new wxChoice( m_panel24, wxID_ANY, wxDefaultPosition, wxSize( 120,-1 ), m_vaeChoices, 0 );
	m_vae->SetSelection( 0 );
	m_vae->Enable( false );

	bSizer131->Add( m_vae, 1, wxALL|wxEXPAND, 5 );

	vaeOnCpu = new wxCheckBox( m_panel24, wxID_ANY, _("CPU"), wxDefaultPosition, wxDefaultSize, 0 );
	vaeOnCpu->SetToolTip( _("VAE on CPU") );

	bSizer131->Add( vaeOnCpu, 0, wxALL|wxEXPAND, 5 );


	fgSizer2->Add( bSizer131, 1, wxEXPAND, 5 );

	m_vae_tiling = new wxCheckBox( m_panel24, wxID_ANY, _("VAE tiling"), wxDefaultPosition, wxDefaultSize, 0 );
	m_vae_tiling->SetToolTip( _("Process vae in tiles to reduce memory usage") );

	fgSizer2->Add( m_vae_tiling, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_vae_decode_only = new wxCheckBox( m_panel24, wxID_ANY, _("VAE decode only"), wxDefaultPosition, wxDefaultSize, 0 );
	m_vae_decode_only->SetValue(true);
	m_vae_decode_only->Enable( false );
	m_vae_decode_only->SetMinSize( wxSize( 140,-1 ) );

	fgSizer2->Add( m_vae_decode_only, 0, wxALL, 5 );

	m_staticText231 = new wxStaticText( m_panel24, wxID_ANY, _("CFG:"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_staticText231->Wrap( 0 );
	fgSizer2->Add( m_staticText231, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_cfg = new wxSpinCtrlDouble( m_panel24, wxID_ANY, wxT("7.0"), wxDefaultPosition, wxSize( -1,-1 ), wxSP_ARROW_KEYS, 0, 30, 0, 0.1 );
	m_cfg->SetDigits( 1 );
	fgSizer2->Add( m_cfg, 0, wxALL|wxEXPAND, 5 );

	m_staticText234 = new wxStaticText( m_panel24, wxID_ANY, _("Clip skip:"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText234->Wrap( 0 );
	m_staticText234->SetToolTip( _("ignore last layers of CLIP network; 1 ignores none, 2 ignores one layer (default: -1)\n<= 0 represents unspecified, will be 1 for SD1.x, 2 for SD2.x") );

	fgSizer2->Add( m_staticText234, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer129;
	bSizer129 = new wxBoxSizer( wxHORIZONTAL );

	m_clip_skip = new wxSpinCtrl( m_panel24, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( -1,-1 ), wxSP_ARROW_KEYS, -1, 2, -1 );
	m_clip_skip->SetToolTip( _("ignore last layers of CLIP network; 1 ignores none, 2 ignores one layer (default: -1)\n<= 0 represents unspecified, will be 1 for SD1.x, 2 for SD2.x") );

	bSizer129->Add( m_clip_skip, 0, wxALL|wxEXPAND, 5 );

	clipOnCpu = new wxCheckBox( m_panel24, wxID_ANY, _("CPU"), wxDefaultPosition, wxDefaultSize, 0 );
	clipOnCpu->SetToolTip( _("CLIP on CPU") );

	bSizer129->Add( clipOnCpu, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	fgSizer2->Add( bSizer129, 1, wxEXPAND, 5 );

	m_staticText20 = new wxStaticText( m_panel24, wxID_ANY, _("TAESD:"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText20->Wrap( -1 );
	m_staticText20->SetToolTip( _("Using Tiny AutoEncoder for fast decoding (low quality)") );

	fgSizer2->Add( m_staticText20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_taesdChoices;
	m_taesd = new wxChoice( m_panel24, wxID_ANY, wxDefaultPosition, wxSize( 180,-1 ), m_taesdChoices, 0 );
	m_taesd->SetSelection( 0 );
	m_taesd->Enable( false );

	fgSizer2->Add( m_taesd, 1, wxALL|wxEXPAND, 5 );

	m_staticText15 = new wxStaticText( m_panel24, wxID_ANY, _("Batch:"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText15->Wrap( -1 );
	m_staticText15->SetToolTip( _("number of images to generate.") );

	fgSizer2->Add( m_staticText15, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_batch_count = new wxSpinCtrl( m_panel24, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxSP_ARROW_KEYS, 1, 1000, 1 );
	m_batch_count->SetToolTip( _("number of images to generate.") );

	fgSizer2->Add( m_batch_count, 0, wxALL|wxEXPAND, 5 );

	m_staticText163 = new wxStaticText( m_panel24, wxID_ANY, _("Sampler:"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText163->Wrap( 0 );
	m_staticText163->SetToolTip( _("sampling method (default: \"euler_a\")") );

	fgSizer2->Add( m_staticText163, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_samplerChoices;
	m_sampler = new wxChoice( m_panel24, wxID_ANY, wxDefaultPosition, wxSize( 180,-1 ), m_samplerChoices, 0 );
	m_sampler->SetSelection( 0 );
	m_sampler->SetToolTip( _("sampling method (default: \"euler_a\")") );

	fgSizer2->Add( m_sampler, 0, wxALL|wxEXPAND, 5 );

	m_schedulertext = new wxStaticText( m_panel24, wxID_ANY, _("Scheduler:"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_schedulertext->Wrap( -1 );
	fgSizer2->Add( m_schedulertext, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_schedulerChoices;
	m_scheduler = new wxChoice( m_panel24, wxID_ANY, wxDefaultPosition, wxSize( 180,-1 ), m_schedulerChoices, 0 );
	m_scheduler->SetSelection( 0 );
	m_scheduler->SetToolTip( _("Weight type. If not specified, the default is the type of the weight file.") );

	fgSizer2->Add( m_scheduler, 0, wxALL|wxEXPAND, 5 );

	m_staticText233 = new wxStaticText( m_panel24, wxID_ANY, _("Seed:"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText233->Wrap( 0 );
	fgSizer2->Add( m_staticText233, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer128;
	bSizer128 = new wxBoxSizer( wxHORIZONTAL );

	m_seed = new wxSpinCtrl( m_panel24, wxID_ANY, wxT("-1"), wxDefaultPosition, wxSize( -1,-1 ), 0, -1, 999999999, -1 );
	bSizer128->Add( m_seed, 0, wxALL|wxEXPAND, 5 );

	m_random_seed = new wxBitmapButton( m_panel24, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_random_seed->SetBitmap( dice_four_png_to_wx_bitmap() );
	m_random_seed->SetToolTip( _("Generate a random seed") );

	bSizer128->Add( m_random_seed, 0, wxALIGN_CENTER_VERTICAL, 5 );


	fgSizer2->Add( bSizer128, 1, wxEXPAND, 5 );

	m_staticText14 = new wxStaticText( m_panel24, wxID_ANY, _("Steps:"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText14->Wrap( -1 );
	m_staticText14->SetToolTip( _("number of sample steps (default: 20)") );

	fgSizer2->Add( m_staticText14, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_steps = new wxSpinCtrl( m_panel24, wxID_ANY, wxT("20"), wxDefaultPosition, wxSize( 180,-1 ), 0, 0, 100, 20 );
	m_steps->SetToolTip( _("number of sample steps (default: 20)") );

	fgSizer2->Add( m_steps, 0, wxALL|wxEXPAND, 5 );

	m_staticText237 = new wxStaticText( m_panel24, wxID_ANY, _("Width:"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText237->Wrap( 0 );
	fgSizer2->Add( m_staticText237, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_width = new wxSpinCtrl( m_panel24, wxID_ANY, wxT("512"), wxDefaultPosition, wxSize( 142,-1 ), wxSP_ARROW_KEYS, 256, 2048, 512 );
	fgSizer2->Add( m_width, 0, wxALL|wxEXPAND, 5 );

	m_staticText239 = new wxStaticText( m_panel24, wxID_ANY, _("Height:"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText239->Wrap( 0 );
	fgSizer2->Add( m_staticText239, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer127;
	bSizer127 = new wxBoxSizer( wxHORIZONTAL );

	m_height = new wxSpinCtrl( m_panel24, wxID_ANY, wxT("512"), wxDefaultPosition, wxSize( 142,-1 ), wxSP_ARROW_KEYS, 256, 2048, 509 );
	bSizer127->Add( m_height, 0, wxALL|wxEXPAND, 5 );

	m_button7 = new wxBitmapButton( m_panel24, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_button7->SetBitmap( replace_png_to_wx_bitmap() );
	m_button7->SetToolTip( _("Swap the resolution") );

	bSizer127->Add( m_button7, 0, wxALIGN_CENTER_VERTICAL, 5 );


	fgSizer2->Add( bSizer127, 1, wxEXPAND, 0 );

	wxBoxSizer* bSizer126;
	bSizer126 = new wxBoxSizer( wxHORIZONTAL );

	m_save_preset = new wxBitmapButton( m_panel24, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_save_preset->SetBitmap( disk_png_to_wx_bitmap() );
	m_save_preset->SetToolTip( _("Save the current settings into preset") );
	m_save_preset->SetHelpText( _("Save the current settings into a new preset, or overwrite an exists one") );

	bSizer126->Add( m_save_preset, 0, wxALL, 5 );

	m_load_preset = new wxBitmapButton( m_panel24, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_load_preset->SetBitmap( file_import_png_to_wx_bitmap() );
	m_load_preset->SetToolTip( _("Load the selected preset") );
	m_load_preset->SetHelpText( _("Load a selected preset. The preset settings will over write the current generation settings") );

	bSizer126->Add( m_load_preset, 0, wxALL, 5 );


	fgSizer2->Add( bSizer126, 0, wxALIGN_RIGHT, 0 );

	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxHORIZONTAL );

	wxArrayString m_preset_listChoices;
	m_preset_list = new wxChoice( m_panel24, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_preset_listChoices, 0 );
	m_preset_list->SetSelection( 0 );
	m_preset_list->Enable( false );

	bSizer91->Add( m_preset_list, 1, wxALL|wxEXPAND, 5 );

	m_delete_preset = new wxBitmapButton( m_panel24, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_delete_preset->SetBitmap( trash_png_to_wx_bitmap() );
	m_delete_preset->Enable( false );

	bSizer91->Add( m_delete_preset, 0, wxALL, 5 );


	fgSizer2->Add( bSizer91, 1, wxEXPAND, 5 );

	m_staticText46 = new wxStaticText( m_panel24, wxID_ANY, _("Low res.:"), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	m_staticText46->Wrap( -1 );
	m_staticText46->SetToolTip( _("Lower resolutions for SD1.x") );

	fgSizer2->Add( m_staticText46, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxString m_sd15ResChoices[] = { _("Select one"), _("640x384"), _("512x512"), _("512x768") };
	int m_sd15ResNChoices = sizeof( m_sd15ResChoices ) / sizeof( wxString );
	m_sd15Res = new wxChoice( m_panel24, wxID_ANY, wxDefaultPosition, wxSize( 160,-1 ), m_sd15ResNChoices, m_sd15ResChoices, 0 );
	m_sd15Res->SetSelection( 0 );
	m_sd15Res->SetToolTip( _("Lower resolutions for SD1.x") );

	fgSizer2->Add( m_sd15Res, 0, wxALL|wxEXPAND, 5 );

	m_staticText461 = new wxStaticText( m_panel24, wxID_ANY, _("High Res.:"), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	m_staticText461->Wrap( -1 );
	m_staticText461->SetToolTip( _("Higher resolutions for FLUX/SDXL/SD3.5") );

	fgSizer2->Add( m_staticText461, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxString m_sdXlresChoices[] = { _("Select one"), _("1024x1024"), _("1152x896"), _("1216x832"), _("1344x768"), _("1536x640") };
	int m_sdXlresNChoices = sizeof( m_sdXlresChoices ) / sizeof( wxString );
	m_sdXlres = new wxChoice( m_panel24, wxID_ANY, wxDefaultPosition, wxSize( 160,-1 ), m_sdXlresNChoices, m_sdXlresChoices, 0 );
	m_sdXlres->SetSelection( 0 );
	m_sdXlres->SetToolTip( _("Higher resolutions for FLUX/SDXL/SD3.5") );

	fgSizer2->Add( m_sdXlres, 0, wxALL|wxEXPAND, 5 );

	m_staticText49 = new wxStaticText( m_panel24, wxID_ANY, _("Prompt presets:"), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	m_staticText49->Wrap( -1 );
	fgSizer2->Add( m_staticText49, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_promptPresetsChoices;
	m_promptPresets = new wxChoice( m_panel24, wxID_ANY, wxDefaultPosition, wxSize( 160,-1 ), m_promptPresetsChoices, 0 );
	m_promptPresets->SetSelection( 0 );
	m_promptPresets->Enable( false );

	fgSizer2->Add( m_promptPresets, 0, wxALL|wxEXPAND, 5 );

	m_queue = new wxButton( m_panel24, wxID_ANY, _("Queue"), wxDefaultPosition, wxDefaultSize, 0 );

	m_queue->SetBitmap( play_png_to_wx_bitmap() );
	m_queue->Enable( false );

	fgSizer2->Add( m_queue, 1, wxALL|wxEXPAND, 5 );


	m_panel24->SetSizer( fgSizer2 );
	m_panel24->Layout();
	bSizer1108->Add( m_panel24, 0, wxALL|wxEXPAND, 0 );

	m_fluxPanel = new wxPanel( m_rightMainPanel, wxID_ANY, wxDefaultPosition, wxSize( 300,-1 ), wxBORDER_NONE|wxTAB_TRAVERSAL );
	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer( 10, 0, 0, 0 );

	m_bpButton25 = new wxBitmapButton( m_fluxPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_bpButton25->SetBitmap( interrogation_png_to_wx_bitmap() );
	m_bpButton25->SetToolTip( _("What is this?") );

	gSizer4->Add( m_bpButton25, 0, wxALL, 5 );


	gSizer4->Add( 10, 1, 0, 0, 0 );

	m_staticText522 = new wxStaticText( m_fluxPanel, wxID_ANY, _("SLG scale:"), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	m_staticText522->Wrap( -1 );
	m_staticText522->SetToolTip( _("skip layer guidance (SLG) scale, only for DiT models: (default: 0.0)") );

	gSizer4->Add( m_staticText522, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	slgScale = new wxSpinCtrlDouble( m_fluxPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 7.000000, 0.5 );
	slgScale->SetDigits( 1 );
	slgScale->SetToolTip( _("skip layer guidance (SLG) scale, only for DiT models: (default: 0.0)") );

	gSizer4->Add( slgScale, 0, wxALL, 5 );

	m_staticText5221 = new wxStaticText( m_fluxPanel, wxID_ANY, _("Skip layers:"), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	m_staticText5221->Wrap( -1 );
	m_staticText5221->SetToolTip( _("Layers to skip for SLG steps: (default: [7,8,9])") );

	gSizer4->Add( m_staticText5221, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_skipLayers = new wxTextCtrl( m_fluxPanel, wxID_ANY, _("7,8,9"), wxDefaultPosition, wxDefaultSize, 0 );
	m_skipLayers->SetToolTip( _("Layers to skip for SLG steps: (default: [7,8,9])") );

	gSizer4->Add( m_skipLayers, 0, wxALL, 5 );

	m_staticText52211 = new wxStaticText( m_fluxPanel, wxID_ANY, _("Skip Layer Start:"), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	m_staticText52211->Wrap( -1 );
	m_staticText52211->SetToolTip( _("SLG enabling point: (default: 0.01)\nSLG will be enabled at step int([STEPS]*[START]) and disabled at int([STEPS]*[END])") );

	gSizer4->Add( m_staticText52211, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	skipLayerStart = new wxSpinCtrlDouble( m_fluxPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.01, 1, 0.01, 0.01 );
	skipLayerStart->SetDigits( 2 );
	skipLayerStart->SetToolTip( _("SLG enabling point: (default: 0.01)\nSLG will be enabled at step int([STEPS]*[START]) and disabled at int([STEPS]*[END])") );

	gSizer4->Add( skipLayerStart, 0, wxALL, 5 );

	m_staticText522111 = new wxStaticText( m_fluxPanel, wxID_ANY, _("Skip Layer End:"), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	m_staticText522111->Wrap( -1 );
	m_staticText522111->SetToolTip( _("SLG disabling point: (default: 0.2)\nSLG will be enabled at step int([STEPS]*[START]) and disabled at int([STEPS]*[END])") );

	gSizer4->Add( m_staticText522111, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	skipLayerEnd = new wxSpinCtrlDouble( m_fluxPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.01, 1, 0.2, 0.01 );
	skipLayerEnd->SetDigits( 2 );
	skipLayerEnd->SetToolTip( _("SLG disabling point: (default: 0.2)\nSLG will be enabled at step int([STEPS]*[START]) and disabled at int([STEPS]*[END])") );

	gSizer4->Add( skipLayerEnd, 0, wxALL, 5 );

	m_staticText52111 = new wxStaticText( m_fluxPanel, wxID_ANY, _("Diffusion Model:"), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	m_staticText52111->Wrap( -1 );
	m_staticText52111->SetToolTip( _("Diffusion models, for example FLUX") );

	gSizer4->Add( m_staticText52111, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_filePickerDiffusionModel = new wxFilePickerCtrl( m_fluxPanel, wxID_ANY, wxEmptyString, _("Select a file"), _("Model files (*.safetensors;*.gguf)|*.safetensors;*.gguf"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_SMALL|wxFLP_USE_TEXTCTRL );
	gSizer4->Add( m_filePickerDiffusionModel, 0, wxALL, 5 );

	m_cleanDiffusionModel = new wxBitmapButton( m_fluxPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_cleanDiffusionModel->SetBitmap( trash_png_to_wx_bitmap() );
	m_cleanDiffusionModel->SetToolTip( _("Clean models") );

	gSizer4->Add( m_cleanDiffusionModel, 0, wxALL, 5 );

	diffusionFlashAttn = new wxCheckBox( m_fluxPanel, wxID_ANY, _("Flash attention"), wxDefaultPosition, wxDefaultSize, 0 );
	diffusionFlashAttn->SetToolTip( _("Use flash attention in the diffusion model. This will reduce significantly the memory uage (for low vram).\nMight lower quality, since it implies converting k and v to f16.\nThis might crash if it is not supported by the backend.\n\n") );

	gSizer4->Add( diffusionFlashAttn, 0, wxALL, 5 );

	m_staticText52 = new wxStaticText( m_fluxPanel, wxID_ANY, _("ClipL:"), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	m_staticText52->Wrap( -1 );
	gSizer4->Add( m_staticText52, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_filePickerClipL = new wxFilePickerCtrl( m_fluxPanel, wxID_ANY, wxEmptyString, _("Select a file"), _("Model files (*.safetensors;*.gguf)|*.safetensors;*.gguf"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_SMALL|wxFLP_USE_TEXTCTRL );
	gSizer4->Add( m_filePickerClipL, 0, wxALL, 5 );

	m_staticText521 = new wxStaticText( m_fluxPanel, wxID_ANY, _("ClipG:"), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	m_staticText521->Wrap( -1 );
	gSizer4->Add( m_staticText521, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_filePickerClipG = new wxFilePickerCtrl( m_fluxPanel, wxID_ANY, wxEmptyString, _("Select a file"), _("Model files (*.safetensors;*.gguf)|*.safetensors;*.gguf"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_SMALL|wxFLP_USE_TEXTCTRL );
	gSizer4->Add( m_filePickerClipG, 0, wxALL, 5 );

	m_staticText5211 = new wxStaticText( m_fluxPanel, wxID_ANY, _("T5XXL:"), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	m_staticText5211->Wrap( -1 );
	gSizer4->Add( m_staticText5211, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_filePickerT5XXL = new wxFilePickerCtrl( m_fluxPanel, wxID_ANY, wxEmptyString, _("Select a file"), _("Model files (*.safetensors;*.gguf)|*.safetensors;*.gguf"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_SMALL|wxFLP_USE_TEXTCTRL );
	gSizer4->Add( m_filePickerT5XXL, 0, wxALL, 5 );


	m_fluxPanel->SetSizer( gSizer4 );
	m_fluxPanel->Layout();
	bSizer1108->Add( m_fluxPanel, 0, wxALL|wxEXPAND, 0 );


	m_rightMainPanel->SetSizer( bSizer1108 );
	m_rightMainPanel->Layout();
	bSizer138->Add( m_rightMainPanel, 0, wxALL|wxEXPAND, 0 );


	m_panel31->SetSizer( bSizer138 );
	m_panel31->Layout();
	bSizer138->Fit( m_panel31 );
	sizer0001->Add( m_panel31, 1, wxEXPAND, 0 );

	logs = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,120 ), wxTE_WORDWRAP|wxTE_READONLY|wxTE_MULTILINE|wxTE_AUTO_URL );
	logs->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	logs->SetMaxSize( wxSize( -1,200 ) );

	sizer0001->Add( logs, 0, wxEXPAND, 5 );


	this->SetSizer( sizer0001 );
	this->Layout();
	m_statusBar166 = this->CreateStatusBar( 3, wxSTB_SIZEGRIP, wxID_ANY );

	this->Centre( wxBOTH );

	// Connect Events
	m_settings->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onSettings ), NULL, this );
	m_refrersh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onModelsRefresh ), NULL, this );
	m_about->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnAboutButton ), NULL, this );
	m_civitai->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnCivitAitButton ), NULL, this );
	m_showWidget->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( mainUI::OnShowWidget ), NULL, this );
	m_stop_background_process->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnStopBackgroundProcess ), NULL, this );
	m_model->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onModelSelect ), NULL, this );
	m_type->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onTypeSelect ), NULL, this );
	m_notebook1302->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( mainUI::m_notebook1302OnNotebookPageChanged ), NULL, this );
	m_start_jobs->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onJobsStart ), NULL, this );
	m_pause_jobs->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onJobsPause ), NULL, this );
	m_delete_all_jobs->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onJobsDelete ), NULL, this );
	m_joblist->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( mainUI::OnJobListItemActivated ), NULL, this );
	m_joblist->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler( mainUI::onContextMenu ), NULL, this );
	m_joblist->Connect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( mainUI::OnJobListItemSelection ), NULL, this );
	m_joblist->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( mainUI::OnJobListItemKeyDown ), NULL, this );
	m_text2img_panel->Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::onTxt2ImgFileDrop ), NULL, this );
	m_prompt->Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::onTxt2ImgFileDrop ), NULL, this );
	m_prompt->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( mainUI::OnPromptText ), NULL, this );
	m_neg_prompt->Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::onTxt2ImgFileDrop ), NULL, this );
	m_neg_prompt->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( mainUI::OnNegPromptText ), NULL, this );
	m_controlnetImageOpen->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::OnControlnetImageOpen ), NULL, this );
	cnOnCpu->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( mainUI::onCnOnCpu ), NULL, this );
	m_controlnetImagePreviewButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnControlnetImagePreviewButton ), NULL, this );
	m_controlnetImageDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnControlnetImageDelete ), NULL, this );
	m_image2image_panel->Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::Onimg2imgDropFile ), NULL, this );
	m_img2imgDiffusionPreview->Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::Onimg2imgDropFile ), NULL, this );
	m_inpaintBrushSizeSlider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintSaveMask->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInpaintSaveMask ), NULL, this );
	m_inpaintInvert->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInpaintInvertMask ), NULL, this );
	m_inpaintResizeToSdSize->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInpaintResizeImage ), NULL, this );
	m_inpaintClearMask->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInpaintCleanMask ), NULL, this );
	m_inpaintOpenMask->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::OnInpaintMaskOpen ), NULL, this );
	m_inPaintBrushStyleCircle->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInPaintBrushStyleToggle ), NULL, this );
	m_inPaintBrushStyleSquare->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInPaintBrushStyleToggle ), NULL, this );
	m_inPaintBrushStyleTriangle->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInPaintBrushStyleToggle ), NULL, this );
	m_img2imPanel->Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::Onimg2imgDropFile ), NULL, this );
	m_img2imPanel->Connect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( mainUI::OnImg2ImgMouseEnter ), NULL, this );
	m_img2imPanel->Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( mainUI::OnImg2ImgMouseLeave ), NULL, this );
	m_img2imPanel->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( mainUI::OnImg2ImgMouseDown ), NULL, this );
	m_img2imPanel->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( mainUI::OnImg2ImgMouseUp ), NULL, this );
	m_img2imPanel->Connect( wxEVT_MOTION, wxMouseEventHandler( mainUI::OnImg2ImgMouseMotion ), NULL, this );
	m_img2imPanel->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( mainUI::OnImg2ImgMouseWheel ), NULL, this );
	m_img2imPanel->Connect( wxEVT_PAINT, wxPaintEventHandler( mainUI::OnImg2ImgPaint ), NULL, this );
	m_img2imPanel->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( mainUI::OnImg2ImgRMouseDown ), NULL, this );
	m_img2imPanel->Connect( wxEVT_RIGHT_UP, wxMouseEventHandler( mainUI::OnImg2ImgRMouseUp ), NULL, this );
	m_inpaintCanvasResizeApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInpaintCanvasResizeApply ), NULL, this );
	m_prompt2->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( mainUI::OnPromptText ), NULL, this );
	m_neg_prompt2->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( mainUI::OnNegPromptText ), NULL, this );
	m_img2imgOpen->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::OnImageOpenFileChanged ), NULL, this );
	m_img2im_preview_img->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImg2ImgPreviewButton ), NULL, this );
	m_delete_initial_img->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnDeleteInitialImage ), NULL, this );
	m_upscaler->Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::OnUpscalerDropFile ), NULL, this );
	m_upscaler_filepicker->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::OnImageOpenFilePickerChanged ), NULL, this );
	m_delete_upscale_image->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnDeleteUpscaleImage ), NULL, this );
	m_upscaler_model->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::OnUpscalerModelSelection ), NULL, this );
	m_upscaler_factor->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( mainUI::OnUpscalerFactorChange ), NULL, this );
	m_upscalerHelp->Connect( wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler( mainUI::OnHtmlLinkClicked ), NULL, this );
	m_imageinfo_preview->Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::OnImageInfoDrop ), NULL, this );
	m_imageInfoPromptTo2txt2img->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoCopyPrompt ), NULL, this );
	m_imageInfoPromptTo2img2img->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoCopyPrompt ), NULL, this );
	m_imageInfoNegPromptTo2txt2img->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoCopyPrompt ), NULL, this );
	m_imageInfoNegPromptTo2img2img->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoCopyPrompt ), NULL, this );
	m_ImageInfoTryFindModel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoTryFindModel ), NULL, this );
	m_imageInfoLoadTotxt->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoLoadTxt2img ), NULL, this );
	m_imageInfoLoadToimg2img->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoLoadImg2img ), NULL, this );
	m_imageInfoOpen->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::OnImageInfoOpen ), NULL, this );
	m_cleanImageInfo->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnCleanImageInfo ), NULL, this );
	m_modelTreeList->Connect( wxEVT_TREELIST_COLUMN_SORTED, wxTreeListEventHandler( mainUI::OnDataModelTreeColSorted ), NULL, this );
	m_modelTreeList->Connect( wxEVT_TREELIST_ITEM_ACTIVATED, wxTreeListEventHandler( mainUI::OnDataModelTreeActivated ), NULL, this );
	m_modelTreeList->Connect( wxEVT_TREELIST_ITEM_CONTEXT_MENU, wxTreeListEventHandler( mainUI::OnDataModelTreeContextMenu ), NULL, this );
	m_modelTreeList->Connect( wxEVT_TREELIST_ITEM_EXPANDED, wxTreeListEventHandler( mainUI::OnDataModelTreeExpanded ), NULL, this );
	m_modelTreeList->Connect( wxEVT_TREELIST_ITEM_EXPANDING, wxTreeListEventHandler( mainUI::OnDataModelTreeExpanding ), NULL, this );
	m_modelTreeList->Connect( wxEVT_TREELIST_SELECTION_CHANGED, wxTreeListEventHandler( mainUI::OnDataModelTreeSelected ), NULL, this );
	m_ModelFavorite->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( mainUI::OnModelFavoriteChange ), NULL, this );
	m_model_details_description->Connect( wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler( mainUI::OnHtmlLinkClicked ), NULL, this );
	m_vae->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onVaeSelect ), NULL, this );
	vaeOnCpu->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( mainUI::onVAEOnCpu ), NULL, this );
	clipOnCpu->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( mainUI::onClipOnCpu ), NULL, this );
	m_sampler->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onSamplerSelect ), NULL, this );
	m_scheduler->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onTypeSelect ), NULL, this );
	m_random_seed->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onRandomGenerateButton ), NULL, this );
	m_width->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( mainUI::OnWHChange ), NULL, this );
	m_width->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( mainUI::OnWHChange ), NULL, this );
	m_height->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( mainUI::OnWHChange ), NULL, this );
	m_height->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( mainUI::OnWHChange ), NULL, this );
	m_button7->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onResolutionSwap ), NULL, this );
	m_save_preset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onSavePreset ), NULL, this );
	m_load_preset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onLoadPreset ), NULL, this );
	m_preset_list->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onSelectPreset ), NULL, this );
	m_delete_preset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onDeletePreset ), NULL, this );
	m_sd15Res->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onSd15ResSelect ), NULL, this );
	m_sdXlres->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onSdXLResSelect ), NULL, this );
	m_queue->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onGenerate ), NULL, this );
	m_bpButton25->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onWhatIsThis ), NULL, this );
	m_filePickerDiffusionModel->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::onFilePickerDiffusionModel ), NULL, this );
	m_cleanDiffusionModel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onCleanDiffusionModel ), NULL, this );
	diffusionFlashAttn->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( mainUI::onDiffusionFlashAttn ), NULL, this );
	m_filePickerClipL->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::onFilePickerClipL ), NULL, this );
	m_filePickerClipG->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::onFilePickerClipG ), NULL, this );
	m_filePickerT5XXL->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::onFilePickerT5XXL ), NULL, this );
}

mainUI::~mainUI()
{
	// Disconnect Events
	m_settings->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onSettings ), NULL, this );
	m_refrersh->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onModelsRefresh ), NULL, this );
	m_about->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnAboutButton ), NULL, this );
	m_civitai->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnCivitAitButton ), NULL, this );
	m_showWidget->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( mainUI::OnShowWidget ), NULL, this );
	m_stop_background_process->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnStopBackgroundProcess ), NULL, this );
	m_model->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onModelSelect ), NULL, this );
	m_type->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onTypeSelect ), NULL, this );
	m_notebook1302->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( mainUI::m_notebook1302OnNotebookPageChanged ), NULL, this );
	m_start_jobs->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onJobsStart ), NULL, this );
	m_pause_jobs->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onJobsPause ), NULL, this );
	m_delete_all_jobs->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onJobsDelete ), NULL, this );
	m_joblist->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( mainUI::OnJobListItemActivated ), NULL, this );
	m_joblist->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler( mainUI::onContextMenu ), NULL, this );
	m_joblist->Disconnect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( mainUI::OnJobListItemSelection ), NULL, this );
	m_joblist->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( mainUI::OnJobListItemKeyDown ), NULL, this );
	m_text2img_panel->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::onTxt2ImgFileDrop ), NULL, this );
	m_prompt->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::onTxt2ImgFileDrop ), NULL, this );
	m_prompt->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( mainUI::OnPromptText ), NULL, this );
	m_neg_prompt->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::onTxt2ImgFileDrop ), NULL, this );
	m_neg_prompt->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( mainUI::OnNegPromptText ), NULL, this );
	m_controlnetImageOpen->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::OnControlnetImageOpen ), NULL, this );
	cnOnCpu->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( mainUI::onCnOnCpu ), NULL, this );
	m_controlnetImagePreviewButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnControlnetImagePreviewButton ), NULL, this );
	m_controlnetImageDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnControlnetImageDelete ), NULL, this );
	m_image2image_panel->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::Onimg2imgDropFile ), NULL, this );
	m_img2imgDiffusionPreview->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::Onimg2imgDropFile ), NULL, this );
	m_inpaintBrushSizeSlider->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintBrushSizeSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( mainUI::OnInpaintBrushSizeSliderScroll ), NULL, this );
	m_inpaintSaveMask->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInpaintSaveMask ), NULL, this );
	m_inpaintInvert->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInpaintInvertMask ), NULL, this );
	m_inpaintResizeToSdSize->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInpaintResizeImage ), NULL, this );
	m_inpaintClearMask->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInpaintCleanMask ), NULL, this );
	m_inpaintOpenMask->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::OnInpaintMaskOpen ), NULL, this );
	m_inPaintBrushStyleCircle->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInPaintBrushStyleToggle ), NULL, this );
	m_inPaintBrushStyleSquare->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInPaintBrushStyleToggle ), NULL, this );
	m_inPaintBrushStyleTriangle->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInPaintBrushStyleToggle ), NULL, this );
	m_img2imPanel->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::Onimg2imgDropFile ), NULL, this );
	m_img2imPanel->Disconnect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( mainUI::OnImg2ImgMouseEnter ), NULL, this );
	m_img2imPanel->Disconnect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( mainUI::OnImg2ImgMouseLeave ), NULL, this );
	m_img2imPanel->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( mainUI::OnImg2ImgMouseDown ), NULL, this );
	m_img2imPanel->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( mainUI::OnImg2ImgMouseUp ), NULL, this );
	m_img2imPanel->Disconnect( wxEVT_MOTION, wxMouseEventHandler( mainUI::OnImg2ImgMouseMotion ), NULL, this );
	m_img2imPanel->Disconnect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( mainUI::OnImg2ImgMouseWheel ), NULL, this );
	m_img2imPanel->Disconnect( wxEVT_PAINT, wxPaintEventHandler( mainUI::OnImg2ImgPaint ), NULL, this );
	m_img2imPanel->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( mainUI::OnImg2ImgRMouseDown ), NULL, this );
	m_img2imPanel->Disconnect( wxEVT_RIGHT_UP, wxMouseEventHandler( mainUI::OnImg2ImgRMouseUp ), NULL, this );
	m_inpaintCanvasResizeApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnInpaintCanvasResizeApply ), NULL, this );
	m_prompt2->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( mainUI::OnPromptText ), NULL, this );
	m_neg_prompt2->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( mainUI::OnNegPromptText ), NULL, this );
	m_img2imgOpen->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::OnImageOpenFileChanged ), NULL, this );
	m_img2im_preview_img->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImg2ImgPreviewButton ), NULL, this );
	m_delete_initial_img->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnDeleteInitialImage ), NULL, this );
	m_upscaler->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::OnUpscalerDropFile ), NULL, this );
	m_upscaler_filepicker->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::OnImageOpenFilePickerChanged ), NULL, this );
	m_delete_upscale_image->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnDeleteUpscaleImage ), NULL, this );
	m_upscaler_model->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::OnUpscalerModelSelection ), NULL, this );
	m_upscaler_factor->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( mainUI::OnUpscalerFactorChange ), NULL, this );
	m_upscalerHelp->Disconnect( wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler( mainUI::OnHtmlLinkClicked ), NULL, this );
	m_imageinfo_preview->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( mainUI::OnImageInfoDrop ), NULL, this );
	m_imageInfoPromptTo2txt2img->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoCopyPrompt ), NULL, this );
	m_imageInfoPromptTo2img2img->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoCopyPrompt ), NULL, this );
	m_imageInfoNegPromptTo2txt2img->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoCopyPrompt ), NULL, this );
	m_imageInfoNegPromptTo2img2img->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoCopyPrompt ), NULL, this );
	m_ImageInfoTryFindModel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoTryFindModel ), NULL, this );
	m_imageInfoLoadTotxt->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoLoadTxt2img ), NULL, this );
	m_imageInfoLoadToimg2img->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnImageInfoLoadImg2img ), NULL, this );
	m_imageInfoOpen->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::OnImageInfoOpen ), NULL, this );
	m_cleanImageInfo->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::OnCleanImageInfo ), NULL, this );
	m_modelTreeList->Disconnect( wxEVT_TREELIST_COLUMN_SORTED, wxTreeListEventHandler( mainUI::OnDataModelTreeColSorted ), NULL, this );
	m_modelTreeList->Disconnect( wxEVT_TREELIST_ITEM_ACTIVATED, wxTreeListEventHandler( mainUI::OnDataModelTreeActivated ), NULL, this );
	m_modelTreeList->Disconnect( wxEVT_TREELIST_ITEM_CONTEXT_MENU, wxTreeListEventHandler( mainUI::OnDataModelTreeContextMenu ), NULL, this );
	m_modelTreeList->Disconnect( wxEVT_TREELIST_ITEM_EXPANDED, wxTreeListEventHandler( mainUI::OnDataModelTreeExpanded ), NULL, this );
	m_modelTreeList->Disconnect( wxEVT_TREELIST_ITEM_EXPANDING, wxTreeListEventHandler( mainUI::OnDataModelTreeExpanding ), NULL, this );
	m_modelTreeList->Disconnect( wxEVT_TREELIST_SELECTION_CHANGED, wxTreeListEventHandler( mainUI::OnDataModelTreeSelected ), NULL, this );
	m_ModelFavorite->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( mainUI::OnModelFavoriteChange ), NULL, this );
	m_model_details_description->Disconnect( wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler( mainUI::OnHtmlLinkClicked ), NULL, this );
	m_vae->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onVaeSelect ), NULL, this );
	vaeOnCpu->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( mainUI::onVAEOnCpu ), NULL, this );
	clipOnCpu->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( mainUI::onClipOnCpu ), NULL, this );
	m_sampler->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onSamplerSelect ), NULL, this );
	m_scheduler->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onTypeSelect ), NULL, this );
	m_random_seed->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onRandomGenerateButton ), NULL, this );
	m_width->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( mainUI::OnWHChange ), NULL, this );
	m_width->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( mainUI::OnWHChange ), NULL, this );
	m_height->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( mainUI::OnWHChange ), NULL, this );
	m_height->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( mainUI::OnWHChange ), NULL, this );
	m_button7->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onResolutionSwap ), NULL, this );
	m_save_preset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onSavePreset ), NULL, this );
	m_load_preset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onLoadPreset ), NULL, this );
	m_preset_list->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onSelectPreset ), NULL, this );
	m_delete_preset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onDeletePreset ), NULL, this );
	m_sd15Res->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onSd15ResSelect ), NULL, this );
	m_sdXlres->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( mainUI::onSdXLResSelect ), NULL, this );
	m_queue->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onGenerate ), NULL, this );
	m_bpButton25->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onWhatIsThis ), NULL, this );
	m_filePickerDiffusionModel->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::onFilePickerDiffusionModel ), NULL, this );
	m_cleanDiffusionModel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainUI::onCleanDiffusionModel ), NULL, this );
	diffusionFlashAttn->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( mainUI::onDiffusionFlashAttn ), NULL, this );
	m_filePickerClipL->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::onFilePickerClipL ), NULL, this );
	m_filePickerClipG->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::onFilePickerClipG ), NULL, this );
	m_filePickerT5XXL->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( mainUI::onFilePickerT5XXL ), NULL, this );

}

Settings::Settings( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxFrame( parent, id, title, pos, size, style, name )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );

	wxBoxSizer* sizer2010;
	sizer2010 = new wxBoxSizer( wxVERTICAL );

	m_notebook1696 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT|wxBORDER_THEME );
	m_path_panel = new wxPanel( m_notebook1696, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizer2011;
	sizer2011 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText172410 = new wxStaticText( m_path_panel, wxID_ANY, _("Models path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText172410->Wrap( 0 );
	m_staticText172410->SetMinSize( wxSize( 150,-1 ) );

	bSizer16->Add( m_staticText172410, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_model_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_model_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer16->Add( m_model_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_openModelsPath = new wxBitmapButton( m_path_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_openModelsPath->SetBitmap( file_import_png_to_wx_bitmap() );
	m_openModelsPath->SetToolTip( _("Open folder") );

	bSizer16->Add( m_openModelsPath, 0, wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer16, 0, wxALL, 5 );

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText18013172027 = new wxStaticText( m_path_panel, wxID_ANY, _("Lora path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18013172027->Wrap( 0 );
	m_staticText18013172027->SetMinSize( wxSize( 150,-1 ) );

	bSizer19->Add( m_staticText18013172027, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_lora_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_lora_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer19->Add( m_lora_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_openLorasPath = new wxBitmapButton( m_path_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_openLorasPath->SetBitmap( file_import_png_to_wx_bitmap() );
	m_openLorasPath->SetToolTip( _("Open folder") );

	bSizer19->Add( m_openLorasPath, 0, wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer19, 0, wxALL, 5 );

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText18013 = new wxStaticText( m_path_panel, wxID_ANY, _("Vae path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18013->Wrap( 0 );
	m_staticText18013->SetMinSize( wxSize( 150,-1 ) );

	bSizer17->Add( m_staticText18013, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_vae_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_vae_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer17->Add( m_vae_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_openVaesPath = new wxBitmapButton( m_path_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_openVaesPath->SetBitmap( file_import_png_to_wx_bitmap() );
	m_openVaesPath->SetToolTip( _("Open folder") );

	bSizer17->Add( m_openVaesPath, 0, wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer17, 0, wxALL, 5 );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1801317202731 = new wxStaticText( m_path_panel, wxID_ANY, _("Embedding path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1801317202731->Wrap( 0 );
	m_staticText1801317202731->SetMinSize( wxSize( 150,-1 ) );

	bSizer18->Add( m_staticText1801317202731, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_embedding_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_embedding_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer18->Add( m_embedding_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_openEmbeddingsPath = new wxBitmapButton( m_path_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_openEmbeddingsPath->SetBitmap( file_import_png_to_wx_bitmap() );
	m_openEmbeddingsPath->SetToolTip( _("Open folder") );

	bSizer18->Add( m_openEmbeddingsPath, 0, wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer18, 0, wxALL, 5 );

	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText21 = new wxStaticText( m_path_panel, wxID_ANY, _("TAESD path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	m_staticText21->SetMinSize( wxSize( 150,-1 ) );

	bSizer23->Add( m_staticText21, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_taesd_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_taesd_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer23->Add( m_taesd_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_openTaesdPath = new wxBitmapButton( m_path_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_openTaesdPath->SetBitmap( file_import_png_to_wx_bitmap() );
	m_openTaesdPath->SetToolTip( _("Open folder") );

	bSizer23->Add( m_openTaesdPath, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	m_bpButton1 = new wxBitmapButton( m_path_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_bpButton1->SetBitmap( interrogation_png_to_wx_bitmap() );
	m_bpButton1->SetToolTip( _("TAESD models you can download from here: https://github.com/madebyollin/taesd/tree/main") );

	bSizer23->Add( m_bpButton1, 0, wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer23, 0, wxALL, 5 );

	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText23 = new wxStaticText( m_path_panel, wxID_ANY, _("Controlnet path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	m_staticText23->SetMinSize( wxSize( 150,-1 ) );

	bSizer35->Add( m_staticText23, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_controlnet_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_controlnet_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer35->Add( m_controlnet_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_openControlnetPath = new wxBitmapButton( m_path_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_openControlnetPath->SetBitmap( file_import_png_to_wx_bitmap() );
	m_openControlnetPath->SetToolTip( _("Open folder") );

	bSizer35->Add( m_openControlnetPath, 0, wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer35, 0, wxALL, 5 );

	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText77 = new wxStaticText( m_path_panel, wxID_ANY, _("ESRGAN path"), wxDefaultPosition, wxSize( 150,-1 ), 0 );
	m_staticText77->Wrap( -1 );
	bSizer81->Add( m_staticText77, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_esrgan_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_esrgan_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer81->Add( m_esrgan_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_openEsrganPath = new wxBitmapButton( m_path_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_openEsrganPath->SetBitmap( file_import_png_to_wx_bitmap() );
	m_openEsrganPath->SetToolTip( _("Open folder") );

	bSizer81->Add( m_openEsrganPath, 0, wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer81, 0, wxALL, 5 );

	m_staticLine223 = new wxStaticLine( m_path_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	sizer2011->Add( m_staticLine223, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText19 = new wxStaticText( m_path_panel, wxID_ANY, _("Presets path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	m_staticText19->SetMinSize( wxSize( 150,-1 ) );

	bSizer20->Add( m_staticText19, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_presets_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_SMALL );
	m_presets_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer20->Add( m_presets_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_openPresetsPath = new wxBitmapButton( m_path_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_openPresetsPath->SetBitmap( file_import_png_to_wx_bitmap() );
	m_openPresetsPath->SetToolTip( _("Open folder") );

	bSizer20->Add( m_openPresetsPath, 0, wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer20, 0, wxALL, 5 );

	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText180131720 = new wxStaticText( m_path_panel, wxID_ANY, _("Images output"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText180131720->Wrap( 0 );
	m_staticText180131720->SetMinSize( wxSize( 150,-1 ) );

	bSizer21->Add( m_staticText180131720, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_images_output = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_SMALL|wxDIRP_DEFAULT_STYLE );
	m_images_output->SetMinSize( wxSize( 200,-1 ) );

	bSizer21->Add( m_images_output, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_openOutputPath = new wxBitmapButton( m_path_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_openOutputPath->SetBitmap( file_import_png_to_wx_bitmap() );
	m_openOutputPath->SetToolTip( _("Open folder") );

	bSizer21->Add( m_openOutputPath, 0, wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer21, 0, wxALL, 5 );


	m_path_panel->SetSizer( sizer2011 );
	m_path_panel->Layout();
	sizer2011->Fit( m_path_panel );
	m_notebook1696->AddPage( m_path_panel, _("Paths"), false );
	m_settings = new wxPanel( m_notebook1696, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizer2017;
	sizer2017 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 0, 2, 0, 0 );

	m_autogen_hash = new wxCheckBox( m_settings, wxID_ANY, _("Generate missing base model hash before diffusion"), wxDefaultPosition, wxDefaultSize, 0 );
	m_autogen_hash->SetValue(true);
	gSizer3->Add( m_autogen_hash, 0, wxALL, 5 );


	sizer2017->Add( gSizer3, 0, wxALL|wxEXPAND, 5 );

	wxGridSizer* sizer2018;
	sizer2018 = new wxGridSizer( 0, 2, 0, 0 );

	m_keep_model_in_memory = new wxCheckBox( m_settings, wxID_ANY, _("Keep model in memory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_keep_model_in_memory->SetValue(true);
	m_keep_model_in_memory->Enable( false );
	m_keep_model_in_memory->SetToolTip( _("Keep model in (v)RAM after diffusions") );
	m_keep_model_in_memory->SetHelpText( _("Keep the model in memory (vRAM or RAM, depends on diffusion type) when it is possible. \n\nText to image and image to image can use the same model. If controlnet model is selected and used, the all new job need to reload the model before run. \nIf type is changed (eg fp16 -> Q4_1), then the model need to be reloaded, but usable with mode diffusion without reload. \n\nIf you disable this option, the model allways unloaded from memory after the job is finished. ") );
	m_keep_model_in_memory->SetMinSize( wxSize( 230,-1 ) );

	sizer2018->Add( m_keep_model_in_memory, 0, wxALIGN_LEFT|wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2017->Add( sizer2018, 0, wxALL|wxEXPAND, 5 );

	wxGridSizer* sizer2019;
	sizer2019 = new wxGridSizer( 0, 2, 0, 0 );

	m_save_all_image = new wxCheckBox( m_settings, wxID_ANY, _("Save all images (intermediate images)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_save_all_image->SetValue(true);
	m_save_all_image->Enable( false );

	sizer2019->Add( m_save_all_image, 0, wxALIGN_LEFT|wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2017->Add( sizer2019, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText16 = new wxStaticText( m_settings, wxID_ANY, _("Output images format"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	m_staticText16->SetMinSize( wxSize( 200,-1 ) );

	bSizer10->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxString m_image_typeChoices[] = { _("JPG"), _("PNG") };
	int m_image_typeNChoices = sizeof( m_image_typeChoices ) / sizeof( wxString );
	m_image_type = new wxChoice( m_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_image_typeNChoices, m_image_typeChoices, 0 );
	m_image_type->SetSelection( 0 );
	bSizer10->Add( m_image_type, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText38 = new wxStaticText( m_settings, wxID_ANY, _("PNG meta data not supported yet"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	m_staticText38->Wrap( -1 );
	m_staticText38->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_LIGHT, false, wxEmptyString ) );

	bSizer10->Add( m_staticText38, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2017->Add( bSizer10, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer112;
	bSizer112 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText61 = new wxStaticText( m_settings, wxID_ANY, _("JPEG quality"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText61->Wrap( -1 );
	m_staticText61->SetMinSize( wxSize( 200,-1 ) );

	bSizer112->Add( m_staticText61, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_image_quality = new wxSlider( m_settings, wxID_ANY, 95, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	m_image_quality->SetToolTip( _("Image output quality, default: 90%") );
	m_image_quality->SetMinSize( wxSize( 250,-1 ) );

	bSizer112->Add( m_image_quality, 0, wxALL, 5 );


	sizer2017->Add( bSizer112, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer113;
	bSizer113 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText62 = new wxStaticText( m_settings, wxID_ANY, _("PNG compression level"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText62->Wrap( -1 );
	m_staticText62->SetMinSize( wxSize( 200,-1 ) );

	bSizer113->Add( m_staticText62, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_png_compression = new wxSlider( m_settings, wxID_ANY, 0, 0, 9, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	m_png_compression->SetToolTip( _("Image output quality, default: 90%") );
	m_png_compression->SetMinSize( wxSize( 250,-1 ) );

	bSizer113->Add( m_png_compression, 0, wxALL, 5 );


	sizer2017->Add( bSizer113, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizer72;
	bSizer72 = new wxBoxSizer( wxVERTICAL );


	sizer2017->Add( bSizer72, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText191 = new wxStaticText( m_settings, wxID_ANY, _("Number of CPU cores"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText191->Wrap( -1 );
	m_staticText191->SetMinSize( wxSize( 200,-1 ) );

	bSizer22->Add( m_staticText191, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_threads = new wxSpinCtrl( m_settings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -1, 100, 2 );
	bSizer22->Add( m_threads, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticNumberOfCores = new wxStaticText( m_settings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticNumberOfCores->Wrap( -1 );
	bSizer22->Add( m_staticNumberOfCores, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2017->Add( bSizer22, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer114;
	bSizer114 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText621 = new wxStaticText( m_settings, wxID_ANY, _("Image name format"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE );
	m_staticText621->Wrap( 140 );
	m_staticText621->SetMinSize( wxSize( 140,-1 ) );

	bSizer114->Add( m_staticText621, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton27 = new wxBitmapButton( m_settings, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_bpButton27->SetBitmap( interrogation_png_to_wx_bitmap() );
	bSizer114->Add( m_bpButton27, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_output_filename_format = new wxTextCtrl( m_settings, wxID_ANY, _("[mode]_[jobid]_[seed]_[width]x[height]_[batch]"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer114->Add( m_output_filename_format, 1, wxALL, 5 );


	sizer2017->Add( bSizer114, 0, wxEXPAND|wxALL, 5 );


	m_settings->SetSizer( sizer2017 );
	m_settings->Layout();
	sizer2017->Fit( m_settings );
	m_notebook1696->AddPage( m_settings, _("Diffusion"), true );
	m_settings_ui = new wxPanel( m_notebook1696, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_show_notifications = new wxCheckBox( m_settings_ui, wxID_ANY, _("Show notifications"), wxDefaultPosition, wxDefaultSize, 0 );
	m_show_notifications->SetValue(true);
	fgSizer3->Add( m_show_notifications, 0, wxALL, 5 );


	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText60 = new wxStaticText( m_settings_ui, wxID_ANY, _("Notification timeout:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText60->Wrap( -1 );
	fgSizer3->Add( m_staticText60, 0, wxALL, 5 );

	m_notification_timeout = new wxSpinCtrl( m_settings_ui, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 30, 120, 60 );
	m_notification_timeout->SetToolTip( _("The timeout is depends on the OS and notification type") );

	fgSizer3->Add( m_notification_timeout, 0, wxALL, 5 );

	m_staticText50 = new wxStaticText( m_settings_ui, wxID_ANY, _("Language:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_staticText50->Wrap( -1 );
	m_staticText50->SetMinSize( wxSize( 160,-1 ) );

	fgSizer3->Add( m_staticText50, 0, wxALL, 5 );

	wxArrayString m_languageChoices;
	m_language = new wxChoice( m_settings_ui, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_languageChoices, 0 );
	m_language->SetSelection( 0 );
	m_language->SetMinSize( wxSize( 160,-1 ) );

	fgSizer3->Add( m_language, 0, wxALL, 5 );

	m_favorite_models_only = new wxCheckBox( m_settings_ui, wxID_ANY, _("Only show favorite models"), wxDefaultPosition, wxDefaultSize, 0 );
	m_favorite_models_only->SetToolTip( _("Only display the models marked as favorites in the model selector of the main GUI") );

	fgSizer3->Add( m_favorite_models_only, 0, wxALL, 5 );


	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );


	m_settings_ui->SetSizer( fgSizer3 );
	m_settings_ui->Layout();
	fgSizer3->Fit( m_settings_ui );
	m_notebook1696->AddPage( m_settings_ui, _("GUI"), false );
	m_panel18 = new wxPanel( m_notebook1696, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer89;
	bSizer89 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer90;
	bSizer90 = new wxBoxSizer( wxVERTICAL );

	m_enableCivitai = new wxCheckBox( m_panel18, wxID_ANY, _("Enable CivitAi features"), wxDefaultPosition, wxDefaultSize, 0 );
	m_enableCivitai->SetValue(true);
	m_enableCivitai->SetToolTip( _("Enable or disable all CivitAi features") );

	bSizer90->Add( m_enableCivitai, 0, wxALL|wxEXPAND, 5 );


	bSizer89->Add( bSizer90, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText42 = new wxStaticText( m_panel18, wxID_ANY, _("CivitAi.com API key"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText42->Wrap( -1 );
	bSizer91->Add( m_staticText42, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_civitai_api_key = new wxTextCtrl( m_panel18, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	bSizer91->Add( m_civitai_api_key, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton15 = new wxBitmapButton( m_panel18, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_bpButton15->SetBitmap( interrogation_png_to_wx_bitmap() );
	m_bpButton15->SetToolTip( _("How to generate API key on CivitAi.com") );
	m_bpButton15->SetHelpText( _("Click to the question mark button to get help") );

	bSizer91->Add( m_bpButton15, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer89->Add( bSizer91, 0, wxEXPAND|wxALL, 5 );


	m_panel18->SetSizer( bSizer89 );
	m_panel18->Layout();
	bSizer89->Fit( m_panel18 );
	m_notebook1696->AddPage( m_panel18, _("CivitAi"), false );

	sizer2010->Add( m_notebook1696, 1, wxALL|wxEXPAND, 5 );

	m_save = new wxButton( this, wxID_ANY, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );

	m_save->SetBitmap( disk_png_to_wx_bitmap() );
	sizer2010->Add( m_save, 0, wxALL, 5 );


	this->SetSizer( sizer2010 );
	this->Layout();
	sizer2010->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_openModelsPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openLorasPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openVaesPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openEmbeddingsPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openTaesdPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_bpButton1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnTAESDHelpClick ), NULL, this );
	m_openControlnetPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openEsrganPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openPresetsPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openOutputPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_png_compression->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_bpButton27->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOutputFileNameFormatHelpClick ), NULL, this );
	m_output_filename_format->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Settings::OnOutputFilenameText ), NULL, this );
	m_show_notifications->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( Settings::onShowNotificationCheck ), NULL, this );
	m_bpButton15->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnCivitaiHelpButton ), NULL, this );
	m_save->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::onSave ), NULL, this );
}

Settings::~Settings()
{
	// Disconnect Events
	m_openModelsPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openLorasPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openVaesPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openEmbeddingsPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openTaesdPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_bpButton1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnTAESDHelpClick ), NULL, this );
	m_openControlnetPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openEsrganPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openPresetsPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_openOutputPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOpenFolder ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_png_compression->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_png_compression->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( Settings::OnPngCompressionScroll ), NULL, this );
	m_bpButton27->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOutputFileNameFormatHelpClick ), NULL, this );
	m_output_filename_format->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Settings::OnOutputFilenameText ), NULL, this );
	m_show_notifications->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( Settings::onShowNotificationCheck ), NULL, this );
	m_bpButton15->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnCivitaiHelpButton ), NULL, this );
	m_save->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::onSave ), NULL, this );

}

ImageDialog::ImageDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxDialog( parent, id, title, pos, size, style, name )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxVERTICAL );

	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer27->Add( m_bitmap, 1, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizer27 );
	this->Layout();
	bSizer27->Fit( this );

	this->Centre( wxBOTH );
}

ImageDialog::~ImageDialog()
{
}

AboutDialog::AboutDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxVERTICAL );

	m_about = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_NO_SELECTION|wxHW_SCROLLBAR_AUTO );
	bSizer71->Add( m_about, 1, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizer71 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_about->Connect( wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler( AboutDialog::m_aboutOnHtmlLinkClicked ), NULL, this );
}

AboutDialog::~AboutDialog()
{
	// Disconnect Events
	m_about->Disconnect( wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler( AboutDialog::m_aboutOnHtmlLinkClicked ), NULL, this );

}

CivitAiWindow::CivitAiWindow( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer82;
	bSizer82 = new wxBoxSizer( wxVERTICAL );

	m_panel16 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer87;
	bSizer87 = new wxBoxSizer( wxHORIZONTAL );

	m_splitter3 = new wxSplitterWindow( m_panel16, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DBORDER );
	m_splitter3->Connect( wxEVT_IDLE, wxIdleEventHandler( CivitAiWindow::m_splitter3OnIdle ), NULL, this );
	m_splitter3->SetMinimumPaneSize( 400 );

	m_panel25 = new wxPanel( m_splitter3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer88;
	bSizer88 = new wxBoxSizer( wxVERTICAL );

	m_model_description = new wxHtmlWindow( m_panel25, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO|wxBORDER_THEME );
	bSizer88->Add( m_model_description, 2, wxEXPAND|wxALL, 5 );

	m_staticText41 = new wxStaticText( m_panel25, wxID_ANY, _("Versions"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	m_staticText41->Wrap( -1 );
	m_staticText41->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	bSizer88->Add( m_staticText41, 0, wxALL|wxEXPAND, 5 );

	m_model_details = new wxDataViewListCtrl( m_panel25, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES|wxDV_SINGLE|wxDV_VARIABLE_LINE_HEIGHT|wxDV_VERT_RULES|wxBORDER_DEFAULT );
	m_dataViewListColumn111 = m_model_details->AppendTextColumn( _("Name"), wxDATAVIEW_CELL_INERT, 200, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	m_dataViewListColumn22 = m_model_details->AppendTextColumn( _("baseModel"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn21 = m_model_details->AppendTextColumn( _("baseModelType"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn21->GetRenderer()->EnableEllipsize( wxELLIPSIZE_NONE );
	m_dataViewListColumn33 = m_model_details->AppendTextColumn( _("Published"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	m_dataViewListColumn34 = m_model_details->AppendTextColumn( _("Downloads"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	m_dataViewListColumn221 = m_model_details->AppendTextColumn( _("Status"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	bSizer88->Add( m_model_details, 1, wxALL|wxEXPAND, 5 );

	m_model_version_description = new wxHtmlWindow( m_panel25, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO|wxBORDER_THEME );
	bSizer88->Add( m_model_version_description, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer93;
	bSizer93 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText40 = new wxStaticText( m_panel25, wxID_ANY, _("Files"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	m_staticText40->Wrap( -1 );
	m_staticText40->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	bSizer93->Add( m_staticText40, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_model_download = new wxButton( m_panel25, wxID_ANY, _("Download"), wxDefaultPosition, wxDefaultSize, 0 );
	m_model_download->Enable( false );

	bSizer93->Add( m_model_download, 0, wxALL, 5 );


	bSizer88->Add( bSizer93, 0, wxEXPAND, 5 );

	m_model_filelist = new wxDataViewListCtrl( m_panel25, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES|wxDV_SINGLE|wxDV_VARIABLE_LINE_HEIGHT|wxDV_VERT_RULES|wxBORDER_DEFAULT );
	m_dataViewListColumn1111 = m_model_filelist->AppendTextColumn( _("Name"), wxDATAVIEW_CELL_INERT, 200, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	m_dataViewListColumn211 = m_model_filelist->AppendTextColumn( _("Type"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn211->GetRenderer()->EnableEllipsize( wxELLIPSIZE_NONE );
	m_dataViewListColumn23 = m_model_filelist->AppendTextColumn( _("Format"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn24 = m_model_filelist->AppendTextColumn( _("Size"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	m_dataViewListColumn25 = m_model_filelist->AppendTextColumn( _("Fp"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn231 = m_model_filelist->AppendTextColumn( _("Status"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn29 = m_model_filelist->AppendTextColumn( _("Size"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	bSizer88->Add( m_model_filelist, 1, wxALL|wxEXPAND, 5 );


	m_panel25->SetSizer( bSizer88 );
	m_panel25->Layout();
	bSizer88->Fit( m_panel25 );
	m_panel24 = new wxPanel( m_splitter3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer92;
	bSizer92 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer86;
	bSizer86 = new wxBoxSizer( wxHORIZONTAL );

	wxString m_model_typeChoices[] = { _("Checkpoints"), _("LORA"), _("Embeddings") };
	int m_model_typeNChoices = sizeof( m_model_typeChoices ) / sizeof( wxString );
	m_model_type = new wxRadioBox( m_panel24, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_model_typeNChoices, m_model_typeChoices, 3, wxRA_SPECIFY_ROWS );
	m_model_type->SetSelection( 0 );
	m_model_type->SetToolTip( _("Select the model type to filter") );

	bSizer86->Add( m_model_type, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );

	m_civitai_search = new wxTextCtrl( m_panel24, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 210,-1 ), wxTE_PROCESS_ENTER );
	m_civitai_search->SetToolTip( _("Search for model names") );

	bSizer86->Add( m_civitai_search, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );

	m_search = new wxButton( m_panel24, wxID_ANY, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer86->Add( m_search, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer92->Add( bSizer86, 0, wxEXPAND, 5 );

	m_staticText43 = new wxStaticText( m_panel24, wxID_ANY, _("Models"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	m_staticText43->Wrap( -1 );
	m_staticText43->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	bSizer92->Add( m_staticText43, 0, wxALL|wxEXPAND, 5 );

	m_dataViewListCtrl5 = new wxDataViewListCtrl( m_panel24, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES );
	m_dataViewListColumn12 = m_dataViewListCtrl5->AppendTextColumn( _("Name"), wxDATAVIEW_CELL_INERT, 200, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	m_dataViewListColumn13 = m_dataViewListCtrl5->AppendTextColumn( _("Status"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn30 = m_dataViewListCtrl5->AppendTextColumn( _("Type"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn32 = m_dataViewListCtrl5->AppendTextColumn( _("Downloads"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	bSizer92->Add( m_dataViewListCtrl5, 1, wxEXPAND|wxALL, 5 );

	m_staticText431 = new wxStaticText( m_panel24, wxID_ANY, _("Downloads"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	m_staticText431->Wrap( -1 );
	m_staticText431->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	bSizer92->Add( m_staticText431, 0, wxALL|wxEXPAND, 5 );

	m_downloads = new wxDataViewListCtrl( m_panel24, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES );
	m_dataViewListColumn26 = m_downloads->AppendTextColumn( _("File"), wxDATAVIEW_CELL_INERT, 200, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn31 = m_downloads->AppendTextColumn( _("Size"), wxDATAVIEW_CELL_INERT, 200, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn301 = m_downloads->AppendTextColumn( _("Status"), wxDATAVIEW_CELL_INERT, 120, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn28 = m_downloads->AppendProgressColumn( wxEmptyString, wxDATAVIEW_CELL_INERT, 200, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	bSizer92->Add( m_downloads, 1, wxALL|wxEXPAND, 5 );

	m_scrolledWindow4 = new wxScrolledWindow( m_panel24, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow4->SetScrollRate( 5, 5 );
	image_container = new wxBoxSizer( wxHORIZONTAL );


	m_scrolledWindow4->SetSizer( image_container );
	m_scrolledWindow4->Layout();
	image_container->Fit( m_scrolledWindow4 );
	bSizer92->Add( m_scrolledWindow4, 1, wxEXPAND | wxALL, 5 );


	m_panel24->SetSizer( bSizer92 );
	m_panel24->Layout();
	bSizer92->Fit( m_panel24 );
	m_splitter3->SplitVertically( m_panel25, m_panel24, 0 );
	bSizer87->Add( m_splitter3, 1, wxEXPAND, 5 );


	m_panel16->SetSizer( bSizer87 );
	m_panel16->Layout();
	bSizer87->Fit( m_panel16 );
	bSizer82->Add( m_panel16, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer82 );
	this->Layout();
	m_statusBar2 = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );

	this->Centre( wxBOTH );

	// Connect Events
	m_model_description->Connect( wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler( CivitAiWindow::OnHtmlLinkClicked ), NULL, this );
	m_model_details->Connect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( CivitAiWindow::m_model_detailsOnDataViewListCtrlSelectionChanged ), NULL, this );
	m_model_version_description->Connect( wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler( CivitAiWindow::OnHtmlLinkClicked ), NULL, this );
	m_model_download->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CivitAiWindow::m_model_downloadOnButtonClick ), NULL, this );
	m_model_filelist->Connect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( CivitAiWindow::m_model_filelistOnDataViewListCtrlSelectionChanged ), NULL, this );
	m_civitai_search->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CivitAiWindow::m_civitai_searchOnTextEnter ), NULL, this );
	m_search->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CivitAiWindow::m_searchOnButtonClick ), NULL, this );
	m_dataViewListCtrl5->Connect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( CivitAiWindow::m_dataViewListCtrl5OnDataViewListCtrlSelectionChanged ), NULL, this );
}

CivitAiWindow::~CivitAiWindow()
{
	// Disconnect Events
	m_model_description->Disconnect( wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler( CivitAiWindow::OnHtmlLinkClicked ), NULL, this );
	m_model_details->Disconnect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( CivitAiWindow::m_model_detailsOnDataViewListCtrlSelectionChanged ), NULL, this );
	m_model_version_description->Disconnect( wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler( CivitAiWindow::OnHtmlLinkClicked ), NULL, this );
	m_model_download->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CivitAiWindow::m_model_downloadOnButtonClick ), NULL, this );
	m_model_filelist->Disconnect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( CivitAiWindow::m_model_filelistOnDataViewListCtrlSelectionChanged ), NULL, this );
	m_civitai_search->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CivitAiWindow::m_civitai_searchOnTextEnter ), NULL, this );
	m_search->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CivitAiWindow::m_searchOnButtonClick ), NULL, this );
	m_dataViewListCtrl5->Disconnect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( CivitAiWindow::m_dataViewListCtrl5OnDataViewListCtrlSelectionChanged ), NULL, this );

}

DesktopWidget::DesktopWidget( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxFrame( parent, id, title, pos, size, style, name )
{
	this->SetSizeHints( wxDefaultSize, wxSize( 400,68 ) );
	this->SetExtraStyle( wxWS_EX_TRANSIENT );
	this->SetForegroundColour( wxColour( 255, 255, 255 ) );
	this->SetBackgroundColour( wxColour( 77, 77, 77 ) );

	wxBoxSizer* bSizer122;
	bSizer122 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer128;
	bSizer128 = new wxBoxSizer( wxVERTICAL );

	m_currentStatus = new wxStaticText( this, wxID_ANY, _("Current job: none"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_currentStatus->Wrap( -1 );
	m_currentStatus->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );
	m_currentStatus->SetForegroundColour( wxColour( 255, 255, 255 ) );

	bSizer128->Add( m_currentStatus, 1, wxEXPAND, 0 );

	m_currentProgress = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( 320,-1 ), wxGA_HORIZONTAL );
	m_currentProgress->SetValue( 0 );
	m_currentProgress->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );
	m_currentProgress->SetMinSize( wxSize( -1,16 ) );

	bSizer128->Add( m_currentProgress, 1, wxEXPAND, 0 );


	bSizer122->Add( bSizer128, 1, wxALL|wxEXPAND, 10 );


	this->SetSizer( bSizer122 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DesktopWidget::OnClose ) );
	this->Connect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( DesktopWidget::OnMouseEnter ) );
	this->Connect( wxEVT_ICONIZE, wxIconizeEventHandler( DesktopWidget::OnIconize ) );
	this->Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( DesktopWidget::OnMouseLeave ) );
	this->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( DesktopWidget::OnLeftMouseDClick ) );
	this->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( DesktopWidget::OnMouseLeftDown ) );
	this->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( DesktopWidget::OnMouseLeftUp ) );
	this->Connect( wxEVT_MOTION, wxMouseEventHandler( DesktopWidget::OnMouseMotion ) );
	this->Connect( wxEVT_PAINT, wxPaintEventHandler( DesktopWidget::OnWidgetPaint ) );
}

DesktopWidget::~DesktopWidget()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DesktopWidget::OnClose ) );
	this->Disconnect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( DesktopWidget::OnMouseEnter ) );
	this->Disconnect( wxEVT_ICONIZE, wxIconizeEventHandler( DesktopWidget::OnIconize ) );
	this->Disconnect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( DesktopWidget::OnMouseLeave ) );
	this->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( DesktopWidget::OnLeftMouseDClick ) );
	this->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( DesktopWidget::OnMouseLeftDown ) );
	this->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( DesktopWidget::OnMouseLeftUp ) );
	this->Disconnect( wxEVT_MOTION, wxMouseEventHandler( DesktopWidget::OnMouseMotion ) );
	this->Disconnect( wxEVT_PAINT, wxPaintEventHandler( DesktopWidget::OnWidgetPaint ) );

}
