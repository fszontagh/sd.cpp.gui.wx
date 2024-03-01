///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.0.0-0-g0efcecf)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MainWindow.h"

#include "embedded_files/cube.png.h"
#include "embedded_files/dice_four.png.h"
#include "embedded_files/disk.png.h"
#include "embedded_files/file_import.png.h"
#include "embedded_files/forward.png.h"
#include "embedded_files/images.png.h"
#include "embedded_files/interrogation.png.h"
#include "embedded_files/palette.png.h"
#include "embedded_files/pause.png.h"
#include "embedded_files/picture.png.h"
#include "embedded_files/play.png.h"
#include "embedded_files/refresh.png.h"
#include "embedded_files/replace.png.h"
#include "embedded_files/settings.png.h"
#include "embedded_files/text_box_dots.png.h"
#include "embedded_files/trash.png.h"

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

UI::UI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxFrame( parent, id, title, pos, size, style, name )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );

	wxBoxSizer* sizer0001;
	sizer0001 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer104;
	bSizer104 = new wxBoxSizer( wxHORIZONTAL );

	m_panel14 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer83;
	bSizer83 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* sizer0021;
	sizer0021 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer97;
	bSizer97 = new wxBoxSizer( wxHORIZONTAL );

	m_settings = new wxBitmapButton( m_panel14, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_settings->SetBitmap( settings_png_to_wx_bitmap() );
	bSizer97->Add( m_settings, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_refrersh = new wxBitmapButton( m_panel14, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_refrersh->SetBitmap( refresh_png_to_wx_bitmap() );
	bSizer97->Add( m_refrersh, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer0021->Add( bSizer97, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer98;
	bSizer98 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText160 = new wxStaticText( m_panel14, wxID_ANY, wxT("Model:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText160->Wrap( 0 );
	bSizer98->Add( m_staticText160, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_modelChoices;
	m_model = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_modelChoices, 0 );
	m_model->SetSelection( 0 );
	m_model->Enable( false );
	m_model->SetMinSize( wxSize( 200,-1 ) );

	bSizer98->Add( m_model, 1, wxALL|wxEXPAND, 5 );


	sizer0021->Add( bSizer98, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer100;
	bSizer100 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText72 = new wxStaticText( m_panel14, wxID_ANY, wxT("Type:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText72->Wrap( -1 );
	bSizer100->Add( m_staticText72, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_typeChoices;
	m_type = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_typeChoices, 0 );
	m_type->SetSelection( 0 );
	m_type->SetToolTip( wxT("Weight type. If not specified, the default is the type of the weight file.") );

	bSizer100->Add( m_type, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );


	sizer0021->Add( bSizer100, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer99;
	bSizer99 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText161 = new wxStaticText( m_panel14, wxID_ANY, wxT("Vae:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText161->Wrap( 0 );
	bSizer99->Add( m_staticText161, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_vaeChoices;
	m_vae = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_vaeChoices, 0 );
	m_vae->SetSelection( 0 );
	m_vae->Enable( false );
	m_vae->SetMinSize( wxSize( 200,-1 ) );

	bSizer99->Add( m_vae, 1, wxALL|wxEXPAND, 5 );


	sizer0021->Add( bSizer99, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer87;
	bSizer87 = new wxBoxSizer( wxHORIZONTAL );

	m_vae_tiling = new wxCheckBox( m_panel14, wxID_ANY, wxT("VAE tiling"), wxDefaultPosition, wxDefaultSize, 0 );
	m_vae_tiling->SetToolTip( wxT("Process vae in tiles to reduce memory usage") );

	bSizer87->Add( m_vae_tiling, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_vae_decode_only = new wxCheckBox( m_panel14, wxID_ANY, wxT("VAE decode only"), wxDefaultPosition, wxDefaultSize, 0 );
	m_vae_decode_only->SetValue(true);
	m_vae_decode_only->Enable( false );

	bSizer87->Add( m_vae_decode_only, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );


	sizer0021->Add( bSizer87, 1, wxEXPAND, 5 );


	bSizer83->Add( sizer0021, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer85;
	bSizer85 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText231 = new wxStaticText( m_panel14, wxID_ANY, wxT("CFG:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText231->Wrap( 0 );
	bSizer85->Add( m_staticText231, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_cfg = new wxSpinCtrlDouble( m_panel14, wxID_ANY, wxT("7.0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 15, 0, 0.1 );
	m_cfg->SetDigits( 1 );
	bSizer85->Add( m_cfg, 1, wxALL|wxEXPAND, 5 );


	bSizer22->Add( bSizer85, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer86;
	bSizer86 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText234 = new wxStaticText( m_panel14, wxID_ANY, wxT("Clip skip:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText234->Wrap( 0 );
	m_staticText234->SetToolTip( wxT("ignore last layers of CLIP network; 1 ignores none, 2 ignores one layer (default: -1)\n<= 0 represents unspecified, will be 1 for SD1.x, 2 for SD2.x") );

	bSizer86->Add( m_staticText234, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_clip_skip = new wxSpinCtrl( m_panel14, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -1, 2, 0 );
	m_clip_skip->SetToolTip( wxT("ignore last layers of CLIP network; 1 ignores none, 2 ignores one layer (default: -1)\n<= 0 represents unspecified, will be 1 for SD1.x, 2 for SD2.x") );
	m_clip_skip->SetMinSize( wxSize( 45,-1 ) );

	bSizer86->Add( m_clip_skip, 1, wxALL|wxEXPAND, 5 );


	bSizer22->Add( bSizer86, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer88;
	bSizer88 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText20 = new wxStaticText( m_panel14, wxID_ANY, wxT("TAESD:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText20->Wrap( -1 );
	m_staticText20->SetToolTip( wxT("Using Tiny AutoEncoder for fast decoding (low quality)") );

	bSizer88->Add( m_staticText20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_taesdChoices;
	m_taesd = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_taesdChoices, 0 );
	m_taesd->SetSelection( 0 );
	m_taesd->Enable( false );

	bSizer88->Add( m_taesd, 1, wxALL|wxEXPAND, 5 );


	bSizer22->Add( bSizer88, 1, wxEXPAND, 5 );


	bSizer83->Add( bSizer22, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer89;
	bSizer89 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText15 = new wxStaticText( m_panel14, wxID_ANY, wxT("Batch:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText15->Wrap( -1 );
	m_staticText15->SetToolTip( wxT("number of images to generate.") );

	bSizer89->Add( m_staticText15, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_batch_count = new wxSpinCtrl( m_panel14, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000, 1 );
	m_batch_count->SetToolTip( wxT("number of images to generate.") );

	bSizer89->Add( m_batch_count, 1, wxALL|wxEXPAND, 5 );


	bSizer9->Add( bSizer89, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer90;
	bSizer90 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText163 = new wxStaticText( m_panel14, wxID_ANY, wxT("Sampler"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText163->Wrap( 0 );
	m_staticText163->SetToolTip( wxT("sampling method (default: \"euler_a\")") );

	bSizer90->Add( m_staticText163, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_samplerChoices;
	m_sampler = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_samplerChoices, 0 );
	m_sampler->SetSelection( 0 );
	m_sampler->SetToolTip( wxT("sampling method (default: \"euler_a\")") );
	m_sampler->SetMinSize( wxSize( 100,-1 ) );

	bSizer90->Add( m_sampler, 1, wxALL|wxEXPAND, 5 );


	bSizer9->Add( bSizer90, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText17 = new wxStaticText( m_panel14, wxID_ANY, wxT("Presets:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText17->Wrap( -1 );
	bSizer91->Add( m_staticText17, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_save_preset = new wxButton( m_panel14, wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 );

	m_save_preset->SetBitmap( disk_png_to_wx_bitmap() );
	bSizer91->Add( m_save_preset, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_load_preset = new wxButton( m_panel14, wxID_ANY, wxT("Load"), wxDefaultPosition, wxDefaultSize, 0 );

	m_load_preset->SetBitmap( file_import_png_to_wx_bitmap() );
	m_load_preset->Enable( false );

	bSizer91->Add( m_load_preset, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_preset_listChoices;
	m_preset_list = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_preset_listChoices, 0 );
	m_preset_list->SetSelection( 0 );
	m_preset_list->Enable( false );

	bSizer91->Add( m_preset_list, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_delete_preset = new wxBitmapButton( m_panel14, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_delete_preset->SetBitmap( trash_png_to_wx_bitmap() );
	m_delete_preset->Enable( false );

	bSizer91->Add( m_delete_preset, 0, wxALL, 5 );


	bSizer9->Add( bSizer91, 1, wxEXPAND, 5 );


	bSizer83->Add( bSizer9, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* sizer0003;
	sizer0003 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer92;
	bSizer92 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText233 = new wxStaticText( m_panel14, wxID_ANY, wxT("Seed:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText233->Wrap( 0 );
	bSizer92->Add( m_staticText233, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_seed = new wxSpinCtrl( m_panel14, wxID_ANY, wxT("-1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -1, 999999999, -1 );
	bSizer92->Add( m_seed, 1, wxALL|wxEXPAND, 5 );

	m_random_seed = new wxBitmapButton( m_panel14, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_random_seed->SetBitmap( dice_four_png_to_wx_bitmap() );
	bSizer92->Add( m_random_seed, 0, wxALIGN_CENTER_VERTICAL, 5 );


	sizer0003->Add( bSizer92, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer93;
	bSizer93 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText14 = new wxStaticText( m_panel14, wxID_ANY, wxT("Steps:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText14->Wrap( -1 );
	m_staticText14->SetToolTip( wxT("number of sample steps (default: 20)") );

	bSizer93->Add( m_staticText14, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_steps = new wxSpinCtrl( m_panel14, wxID_ANY, wxT("20"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 20 );
	m_steps->SetToolTip( wxT("number of sample steps (default: 20)") );

	bSizer93->Add( m_steps, 1, wxALL|wxEXPAND, 5 );


	sizer0003->Add( bSizer93, 0, wxEXPAND, 5 );


	bSizer83->Add( sizer0003, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer96;
	bSizer96 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer94;
	bSizer94 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText237 = new wxStaticText( m_panel14, wxID_ANY, wxT("Width:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText237->Wrap( 0 );
	bSizer94->Add( m_staticText237, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_width = new wxSpinCtrl( m_panel14, wxID_ANY, wxT("512"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 128, 2048, 508 );
	bSizer94->Add( m_width, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer96->Add( bSizer94, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer95;
	bSizer95 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText239 = new wxStaticText( m_panel14, wxID_ANY, wxT("Height:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText239->Wrap( 0 );
	bSizer95->Add( m_staticText239, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_height = new wxSpinCtrl( m_panel14, wxID_ANY, wxT("512"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 128, 2048, 512 );
	bSizer95->Add( m_height, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_button7 = new wxButton( m_panel14, wxID_ANY, wxT("Swap"), wxDefaultPosition, wxDefaultSize, 0 );

	m_button7->SetBitmap( replace_png_to_wx_bitmap() );
	m_button7->SetToolTip( wxT("Swap width and height") );
	m_button7->SetHelpText( wxT("Swap the resolution width and height") );

	bSizer95->Add( m_button7, 0, wxALL, 5 );


	bSizer96->Add( bSizer95, 0, wxEXPAND, 5 );


	bSizer83->Add( bSizer96, 0, wxALL|wxEXPAND, 5 );


	m_panel14->SetSizer( bSizer83 );
	m_panel14->Layout();
	bSizer83->Fit( m_panel14 );
	bSizer104->Add( m_panel14, 0, wxEXPAND, 5 );

	m_notebook1302 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxSize( -1,200 ), wxBK_DEFAULT );
	m_notebook1302->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	m_notebook1302->SetMinSize( wxSize( -1,200 ) );

	m_jobs_panel = new wxPanel( m_notebook1302, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("jobs") );
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );

	m_start_jobs = new wxButton( m_jobs_panel, wxID_ANY, wxT("Start jobs"), wxDefaultPosition, wxDefaultSize, 0 );

	m_start_jobs->SetBitmap( forward_png_to_wx_bitmap() );
	m_start_jobs->Enable( false );

	bSizer18->Add( m_start_jobs, 0, wxALL, 5 );

	m_pause_jobs = new wxButton( m_jobs_panel, wxID_ANY, wxT("Pause"), wxDefaultPosition, wxDefaultSize, 0 );

	m_pause_jobs->SetBitmap( pause_png_to_wx_bitmap() );
	m_pause_jobs->Enable( false );

	bSizer18->Add( m_pause_jobs, 0, wxALL, 5 );

	m_delete_all_jobs = new wxButton( m_jobs_panel, wxID_ANY, wxT("Delete all"), wxDefaultPosition, wxDefaultSize, 0 );

	m_delete_all_jobs->SetBitmap( trash_png_to_wx_bitmap() );
	m_delete_all_jobs->Enable( false );

	bSizer18->Add( m_delete_all_jobs, 0, wxALL, 5 );


	bSizer17->Add( bSizer18, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer78;
	bSizer78 = new wxBoxSizer( wxHORIZONTAL );

	m_joblist = new wxDataViewListCtrl( m_jobs_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES|wxDV_SINGLE|wxDV_VERT_RULES );
	bSizer78->Add( m_joblist, 1, wxEXPAND, 5 );

	m_listCtrl1 = new wxListCtrl( m_jobs_panel, wxID_ANY, wxDefaultPosition, wxSize( 300,-1 ), wxLC_ALIGN_LEFT|wxLC_ICON|wxLC_NO_HEADER|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer78->Add( m_listCtrl1, 1, wxEXPAND, 5 );


	bSizer17->Add( bSizer78, 1, wxEXPAND, 5 );


	m_jobs_panel->SetSizer( bSizer17 );
	m_jobs_panel->Layout();
	bSizer17->Fit( m_jobs_panel );
	m_notebook1302->AddPage( m_jobs_panel, wxT("Jobs and Images"), false );
	m_text2img_panel = new wxPanel( m_notebook1302, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("txt2txt") );
	m_text2img_panel->DragAcceptFiles( true );

	wxBoxSizer* sizer0004;
	sizer0004 = new wxBoxSizer( wxVERTICAL );

	sizer0004->SetMinSize( wxSize( -1,100 ) );
	wxBoxSizer* bSizer811;
	bSizer811 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer82;
	bSizer82 = new wxBoxSizer( wxVERTICAL );

	m_prompt = new wxTextCtrl( m_text2img_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,300 ), wxTE_WORDWRAP|wxTE_PROCESS_ENTER|wxTE_MULTILINE );
	bSizer82->Add( m_prompt, 1, wxALL|wxEXPAND, 1 );

	m_neg_prompt = new wxTextCtrl( m_text2img_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,300 ), wxTE_WORDWRAP|wxTE_PROCESS_ENTER|wxTE_MULTILINE );
	m_neg_prompt->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	bSizer82->Add( m_neg_prompt, 1, wxALL|wxEXPAND, 1 );


	bSizer82->Add( 0, 0, 1, wxEXPAND, 5 );


	bSizer811->Add( bSizer82, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer331;
	bSizer331 = new wxBoxSizer( wxVERTICAL );

	bSizer331->SetMinSize( wxSize( -1,100 ) );
	m_controlnetImagePreview = new wxStaticBitmap( m_text2img_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxFULL_REPAINT_ON_RESIZE );
	m_controlnetImagePreview->SetBackgroundColour( wxColour( 64, 64, 64 ) );

	bSizer331->Add( m_controlnetImagePreview, 1, wxEXPAND, 5 );


	bSizer811->Add( bSizer331, 1, wxEXPAND, 5 );


	sizer0004->Add( bSizer811, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxHORIZONTAL );

	m_generate2 = new wxButton( m_text2img_panel, wxID_ANY, wxT("Queue"), wxDefaultPosition, wxDefaultSize, 0 );

	m_generate2->SetBitmap( play_png_to_wx_bitmap() );
	m_generate2->Enable( false );

	bSizer34->Add( m_generate2, 0, wxALL, 5 );

	m_controlnetImageOpen = new wxFilePickerCtrl( m_text2img_panel, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("PNG files (*.png)|*.png|JPEG (*.jpg)|*.jpg"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	m_controlnetImageOpen->SetMinSize( wxSize( 260,-1 ) );

	bSizer34->Add( m_controlnetImageOpen, 0, wxALL, 5 );

	wxArrayString m_controlnetModelsChoices;
	m_controlnetModels = new wxChoice( m_text2img_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_controlnetModelsChoices, 0 );
	m_controlnetModels->SetSelection( 0 );
	m_controlnetModels->Enable( false );
	m_controlnetModels->SetMinSize( wxSize( 210,-1 ) );

	bSizer34->Add( m_controlnetModels, 0, wxALL, 5 );

	m_controlnetStrength = new wxSpinCtrlDouble( m_text2img_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 0.9, 0.9, 0.1 );
	m_controlnetStrength->SetDigits( 1 );
	bSizer34->Add( m_controlnetStrength, 0, wxALL, 5 );

	m_controlnetImagePreviewButton = new wxButton( m_text2img_panel, wxID_ANY, wxT("Show full"), wxDefaultPosition, wxDefaultSize, 0 );
	m_controlnetImagePreviewButton->Enable( false );
	m_controlnetImagePreviewButton->SetToolTip( wxT("Show the original controlnet image") );

	bSizer34->Add( m_controlnetImagePreviewButton, 0, wxALL, 5 );

	m_controlnetImageDelete = new wxBitmapButton( m_text2img_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_controlnetImageDelete->SetBitmap( trash_png_to_wx_bitmap() );
	m_controlnetImageDelete->Enable( false );
	m_controlnetImageDelete->SetToolTip( wxT("Remove control image") );

	bSizer34->Add( m_controlnetImageDelete, 0, wxALL, 5 );


	sizer0004->Add( bSizer34, 0, wxEXPAND, 5 );


	m_text2img_panel->SetSizer( sizer0004 );
	m_text2img_panel->Layout();
	sizer0004->Fit( m_text2img_panel );
	m_notebook1302->AddPage( m_text2img_panel, wxT("Text2IMG"), true );
	m_image2image_panel = new wxPanel( m_notebook1302, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("img2img") );
	m_image2image_panel->DragAcceptFiles( true );

	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxHORIZONTAL );

	m_open_image = new wxFilePickerCtrl( m_image2image_panel, wxID_ANY, wxEmptyString, wxT("Select an image"), wxT("PNG files (*.png)|*.png|JPEG (*.jpg)|*.jpg"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_USE_TEXTCTRL );
	bSizer36->Add( m_open_image, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_img2im_preview_img = new wxButton( m_image2image_panel, wxID_ANY, wxT("Show full"), wxDefaultPosition, wxDefaultSize, 0 );
	m_img2im_preview_img->Enable( false );

	bSizer36->Add( m_img2im_preview_img, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_delete_initial_img = new wxBitmapButton( m_image2image_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_delete_initial_img->SetBitmap( trash_png_to_wx_bitmap() );
	bSizer36->Add( m_delete_initial_img, 0, wxALL, 5 );


	bSizer33->Add( bSizer36, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxVERTICAL );

	m_prompt2 = new wxTextCtrl( m_image2image_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_TAB|wxTE_WORDWRAP );
	bSizer37->Add( m_prompt2, 1, wxEXPAND|wxALL, 5 );

	m_neg_prompt2 = new wxTextCtrl( m_image2image_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_TAB|wxTE_WORDWRAP );
	bSizer37->Add( m_neg_prompt2, 1, wxEXPAND|wxALL, 5 );


	bSizer37->Add( 0, 0, 1, wxEXPAND, 5 );


	bSizer33->Add( bSizer37, 1, wxEXPAND, 1 );

	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxHORIZONTAL );

	m_generate1 = new wxButton( m_image2image_panel, wxID_ANY, wxT("Queue"), wxDefaultPosition, wxDefaultSize, 0 );

	m_generate1->SetBitmap( play_png_to_wx_bitmap() );
	m_generate1->Enable( false );

	bSizer38->Add( m_generate1, 0, wxALL, 5 );

	m_staticText24 = new wxStaticText( m_image2image_panel, wxID_ANY, wxT("Strength:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	bSizer38->Add( m_staticText24, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_strength = new wxSpinCtrlDouble( m_image2image_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 0.99, 0.75, 0.1 );
	m_strength->SetDigits( 2 );
	bSizer38->Add( m_strength, 0, wxALL, 5 );


	bSizer33->Add( bSizer38, 0, wxEXPAND, 5 );


	bSizer28->Add( bSizer33, 1, wxEXPAND, 5 );


	bSizer24->Add( bSizer28, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxHORIZONTAL );

	m_img2img_preview = new wxStaticBitmap( m_image2image_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_img2img_preview->SetBackgroundColour( wxColour( 64, 64, 64 ) );
	m_img2img_preview->DragAcceptFiles( true );

	bSizer29->Add( m_img2img_preview, 1, wxEXPAND, 5 );


	bSizer24->Add( bSizer29, 1, wxEXPAND, 5 );


	m_image2image_panel->SetSizer( bSizer24 );
	m_image2image_panel->Layout();
	bSizer24->Fit( m_image2image_panel );
	m_notebook1302->AddPage( m_image2image_panel, wxT("Image2image"), false );
	m_upscaler = new wxPanel( m_notebook1302, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("upscaler") );
	m_upscaler->DragAcceptFiles( true );

	wxBoxSizer* bSizer68;
	bSizer68 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer999;
	bSizer999 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer69;
	bSizer69 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText62 = new wxStaticText( m_upscaler, wxID_ANY, wxT("Source Image"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_staticText62->Wrap( -1 );
	bSizer71->Add( m_staticText62, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_upscaler_filepicker = new wxFilePickerCtrl( m_upscaler, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("PNG files (*.png)|*.png|JPEG (*.jpg)|*.jpg"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	bSizer71->Add( m_upscaler_filepicker, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_delete_upscale_image = new wxBitmapButton( m_upscaler, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_delete_upscale_image->SetBitmap( trash_png_to_wx_bitmap() );
	bSizer71->Add( m_delete_upscale_image, 0, wxALL, 5 );


	bSizer69->Add( bSizer71, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer70;
	bSizer70 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText61 = new wxStaticText( m_upscaler, wxID_ANY, wxT("Upscaler model"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_staticText61->Wrap( -1 );
	bSizer70->Add( m_staticText61, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_upscaler_modelChoices;
	m_upscaler_model = new wxChoice( m_upscaler, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_upscaler_modelChoices, 0 );
	m_upscaler_model->SetSelection( 0 );
	bSizer70->Add( m_upscaler_model, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer69->Add( bSizer70, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer72;
	bSizer72 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText63 = new wxStaticText( m_upscaler, wxID_ANY, wxT("Width:"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_staticText63->Wrap( -1 );
	bSizer72->Add( m_staticText63, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_static_upscaler_width = new wxStaticText( m_upscaler, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_static_upscaler_width->Wrap( -1 );
	bSizer72->Add( m_static_upscaler_width, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText65 = new wxStaticText( m_upscaler, wxID_ANY, wxT("Height:"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_staticText65->Wrap( -1 );
	bSizer72->Add( m_staticText65, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_static_upscaler_height = new wxStaticText( m_upscaler, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_static_upscaler_height->Wrap( -1 );
	bSizer72->Add( m_static_upscaler_height, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer69->Add( bSizer72, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer721;
	bSizer721 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText631 = new wxStaticText( m_upscaler, wxID_ANY, wxT("Width:"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_staticText631->Wrap( -1 );
	bSizer721->Add( m_staticText631, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_static_upscaler_target_width = new wxStaticText( m_upscaler, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_static_upscaler_target_width->Wrap( -1 );
	bSizer721->Add( m_static_upscaler_target_width, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText651 = new wxStaticText( m_upscaler, wxID_ANY, wxT("Height:"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_staticText651->Wrap( -1 );
	bSizer721->Add( m_staticText651, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_static_upscaler_target_height = new wxStaticText( m_upscaler, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_static_upscaler_target_height->Wrap( -1 );
	bSizer721->Add( m_static_upscaler_target_height, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer69->Add( bSizer721, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer73;
	bSizer73 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText67 = new wxStaticText( m_upscaler, wxID_ANY, wxT("Upscale factor"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_staticText67->Wrap( -1 );
	bSizer73->Add( m_staticText67, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_upscaler_factor = new wxSpinCtrl( m_upscaler, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 4, 4 );
	m_upscaler_factor->Enable( false );

	bSizer73->Add( m_upscaler_factor, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer69->Add( bSizer73, 0, wxEXPAND, 5 );


	bSizer999->Add( bSizer69, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer77;
	bSizer77 = new wxBoxSizer( wxHORIZONTAL );

	m_generate_upscaler = new wxButton( m_upscaler, wxID_ANY, wxT("Queue"), wxDefaultPosition, wxDefaultSize, 0 );

	m_generate_upscaler->SetBitmap( play_png_to_wx_bitmap() );
	m_generate_upscaler->Enable( false );

	bSizer77->Add( m_generate_upscaler, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_keep_upscaler_in_memory = new wxCheckBox( m_upscaler, wxID_ANY, wxT("Keep model in memory"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer77->Add( m_keep_upscaler_in_memory, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer999->Add( bSizer77, 0, wxEXPAND, 5 );


	bSizer68->Add( bSizer999, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer1000;
	bSizer1000 = new wxBoxSizer( wxHORIZONTAL );

	m_upscaler_source_image = new wxStaticBitmap( m_upscaler, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_upscaler_source_image->SetBackgroundColour( wxColour( 80, 80, 80 ) );

	bSizer1000->Add( m_upscaler_source_image, 1, wxEXPAND, 5 );


	bSizer68->Add( bSizer1000, 1, wxEXPAND, 5 );


	m_upscaler->SetSizer( bSizer68 );
	m_upscaler->Layout();
	bSizer68->Fit( m_upscaler );
	m_notebook1302->AddPage( m_upscaler, wxT("Upscaler"), false );
	m_models_panel = new wxPanel( m_notebook1302, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("models") );
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer63;
	bSizer63 = new wxBoxSizer( wxHORIZONTAL );

	m_checkbox_lora_filter = new wxCheckBox( m_models_panel, wxID_ANY, wxT("Lora"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkbox_lora_filter->SetValue(true);
	bSizer63->Add( m_checkbox_lora_filter, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_checkbox_filter_checkpoints = new wxCheckBox( m_models_panel, wxID_ANY, wxT("Checkpoints"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkbox_filter_checkpoints->SetValue(true);
	bSizer63->Add( m_checkbox_filter_checkpoints, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_modellist_filter = new wxSearchCtrl( m_models_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	#ifndef __WXMAC__
	m_modellist_filter->ShowSearchButton( true );
	#endif
	m_modellist_filter->ShowCancelButton( false );
	bSizer63->Add( m_modellist_filter, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer20->Add( bSizer63, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizer102;
	bSizer102 = new wxBoxSizer( wxHORIZONTAL );

	m_data_model_list = new wxDataViewListCtrl( m_models_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES|wxDV_ROW_LINES|wxDV_SINGLE|wxFULL_REPAINT_ON_RESIZE );
	bSizer102->Add( m_data_model_list, 1, wxEXPAND, 5 );

	m_listCtrl11 = new wxListCtrl( m_models_panel, wxID_ANY, wxDefaultPosition, wxSize( 300,-1 ), wxLC_ALIGN_LEFT|wxLC_ICON|wxLC_NO_HEADER|wxLC_SINGLE_SEL|wxLC_VRULES );
	m_listCtrl11->SetMinSize( wxSize( 300,-1 ) );

	bSizer102->Add( m_listCtrl11, 1, wxEXPAND, 5 );


	bSizer20->Add( bSizer102, 1, wxEXPAND, 5 );


	m_models_panel->SetSizer( bSizer20 );
	m_models_panel->Layout();
	bSizer20->Fit( m_models_panel );
	m_notebook1302->AddPage( m_models_panel, wxT("Models"), false );

	bSizer104->Add( m_notebook1302, 1, wxALIGN_TOP|wxEXPAND, 0 );


	sizer0001->Add( bSizer104, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer105;
	bSizer105 = new wxBoxSizer( wxVERTICAL );

	logs = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,140 ), wxTE_WORDWRAP|wxTE_READONLY|wxTE_MULTILINE|wxTE_AUTO_URL|wxFULL_REPAINT_ON_RESIZE );
	bSizer105->Add( logs, 0, wxEXPAND, 5 );


	sizer0001->Add( bSizer105, 0, wxEXPAND, 5 );


	this->SetSizer( sizer0001 );
	this->Layout();
	sizer0001->Fit( this );
	m_statusBar166 = this->CreateStatusBar( 1, wxSTB_DEFAULT_STYLE, wxID_ANY );

	this->Centre( wxBOTH );

	// Connect Events
	m_settings->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onSettings ), NULL, this );
	m_refrersh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onModelsRefresh ), NULL, this );
	m_model->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UI::onModelSelect ), NULL, this );
	m_type->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UI::onTypeSelect ), NULL, this );
	m_vae->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UI::onVaeSelect ), NULL, this );
	m_sampler->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UI::onSamplerSelect ), NULL, this );
	m_save_preset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onSavePreset ), NULL, this );
	m_load_preset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onLoadPreset ), NULL, this );
	m_preset_list->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UI::onSelectPreset ), NULL, this );
	m_delete_preset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onDeletePreset ), NULL, this );
	m_random_seed->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onRandomGenerateButton ), NULL, this );
	m_button7->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onResolutionSwap ), NULL, this );
	m_start_jobs->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onJobsStart ), NULL, this );
	m_pause_jobs->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onJobsPause ), NULL, this );
	m_delete_all_jobs->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onJobsDelete ), NULL, this );
	m_joblist->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( UI::OnJobListItemActivated ), NULL, this );
	m_joblist->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler( UI::onContextMenu ), NULL, this );
	m_text2img_panel->Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( UI::onTxt2ImgFileDrop ), NULL, this );
	m_generate2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onGenerate ), NULL, this );
	m_controlnetImageOpen->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( UI::OnControlnetImageOpen ), NULL, this );
	m_controlnetImagePreviewButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::OnControlnetImagePreviewButton ), NULL, this );
	m_controlnetImageDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::OnControlnetImageDelete ), NULL, this );
	m_image2image_panel->Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( UI::Onimg2imgDropFile ), NULL, this );
	m_open_image->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( UI::OnImageOpenFileChanged ), NULL, this );
	m_img2im_preview_img->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::OnImg2ImgPreviewButton ), NULL, this );
	m_delete_initial_img->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::OnDeleteInitialImage ), NULL, this );
	m_generate1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onGenerate ), NULL, this );
	m_img2img_preview->Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( UI::Onimg2imgDropFile ), NULL, this );
	m_upscaler->Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( UI::OnUpscalerDropFile ), NULL, this );
	m_upscaler_filepicker->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( UI::OnImageOpenFilePickerChanged ), NULL, this );
	m_delete_upscale_image->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::OnDeleteUpscaleImage ), NULL, this );
	m_upscaler_model->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UI::OnUpscalerModelSelection ), NULL, this );
	m_upscaler_factor->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( UI::OnUpscalerFactorChange ), NULL, this );
	m_generate_upscaler->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onGenerate ), NULL, this );
	m_checkbox_lora_filter->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( UI::OnCheckboxLoraFilter ), NULL, this );
	m_checkbox_filter_checkpoints->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( UI::OnCheckboxCheckpointFilter ), NULL, this );
	m_modellist_filter->Connect( wxEVT_KEY_UP, wxKeyEventHandler( UI::OnModellistFilterKeyUp ), NULL, this );
	m_data_model_list->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( UI::OnDataModelActivated ), NULL, this );
	m_data_model_list->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler( UI::onContextMenu ), NULL, this );
}

UI::~UI()
{
	// Disconnect Events
	m_settings->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onSettings ), NULL, this );
	m_refrersh->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onModelsRefresh ), NULL, this );
	m_model->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UI::onModelSelect ), NULL, this );
	m_type->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UI::onTypeSelect ), NULL, this );
	m_vae->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UI::onVaeSelect ), NULL, this );
	m_sampler->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UI::onSamplerSelect ), NULL, this );
	m_save_preset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onSavePreset ), NULL, this );
	m_load_preset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onLoadPreset ), NULL, this );
	m_preset_list->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UI::onSelectPreset ), NULL, this );
	m_delete_preset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onDeletePreset ), NULL, this );
	m_random_seed->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onRandomGenerateButton ), NULL, this );
	m_button7->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onResolutionSwap ), NULL, this );
	m_start_jobs->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onJobsStart ), NULL, this );
	m_pause_jobs->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onJobsPause ), NULL, this );
	m_delete_all_jobs->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onJobsDelete ), NULL, this );
	m_joblist->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( UI::OnJobListItemActivated ), NULL, this );
	m_joblist->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler( UI::onContextMenu ), NULL, this );
	m_text2img_panel->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( UI::onTxt2ImgFileDrop ), NULL, this );
	m_generate2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onGenerate ), NULL, this );
	m_controlnetImageOpen->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( UI::OnControlnetImageOpen ), NULL, this );
	m_controlnetImagePreviewButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::OnControlnetImagePreviewButton ), NULL, this );
	m_controlnetImageDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::OnControlnetImageDelete ), NULL, this );
	m_image2image_panel->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( UI::Onimg2imgDropFile ), NULL, this );
	m_open_image->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( UI::OnImageOpenFileChanged ), NULL, this );
	m_img2im_preview_img->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::OnImg2ImgPreviewButton ), NULL, this );
	m_delete_initial_img->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::OnDeleteInitialImage ), NULL, this );
	m_generate1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onGenerate ), NULL, this );
	m_img2img_preview->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( UI::Onimg2imgDropFile ), NULL, this );
	m_upscaler->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( UI::OnUpscalerDropFile ), NULL, this );
	m_upscaler_filepicker->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( UI::OnImageOpenFilePickerChanged ), NULL, this );
	m_delete_upscale_image->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::OnDeleteUpscaleImage ), NULL, this );
	m_upscaler_model->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UI::OnUpscalerModelSelection ), NULL, this );
	m_upscaler_factor->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( UI::OnUpscalerFactorChange ), NULL, this );
	m_generate_upscaler->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UI::onGenerate ), NULL, this );
	m_checkbox_lora_filter->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( UI::OnCheckboxLoraFilter ), NULL, this );
	m_checkbox_filter_checkpoints->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( UI::OnCheckboxCheckpointFilter ), NULL, this );
	m_modellist_filter->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( UI::OnModellistFilterKeyUp ), NULL, this );
	m_data_model_list->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( UI::OnDataModelActivated ), NULL, this );
	m_data_model_list->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler( UI::onContextMenu ), NULL, this );

}

Settings::Settings( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxFrame( parent, id, title, pos, size, style, name )
{
	this->SetSizeHints( wxSize( -1,500 ), wxDefaultSize );

	wxBoxSizer* sizer2010;
	sizer2010 = new wxBoxSizer( wxVERTICAL );

	m_notebook1696 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );
	m_path_panel = new wxPanel( m_notebook1696, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizer2011;
	sizer2011 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText172410 = new wxStaticText( m_path_panel, wxID_ANY, wxT("Models path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText172410->Wrap( 0 );
	m_staticText172410->SetMinSize( wxSize( 150,-1 ) );

	bSizer16->Add( m_staticText172410, 0, wxALL, 5 );

	m_model_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_model_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer16->Add( m_model_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer16, 0, wxALL, 5 );

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText18013172027 = new wxStaticText( m_path_panel, wxID_ANY, wxT("Lora path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18013172027->Wrap( 0 );
	m_staticText18013172027->SetMinSize( wxSize( 150,-1 ) );

	bSizer19->Add( m_staticText18013172027, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_lora_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_lora_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer19->Add( m_lora_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer19, 0, wxALL, 5 );

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText18013 = new wxStaticText( m_path_panel, wxID_ANY, wxT("Vae path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18013->Wrap( 0 );
	m_staticText18013->SetMinSize( wxSize( 150,-1 ) );

	bSizer17->Add( m_staticText18013, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_vae_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_vae_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer17->Add( m_vae_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer17, 0, wxALL, 5 );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1801317202731 = new wxStaticText( m_path_panel, wxID_ANY, wxT("Embedding path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1801317202731->Wrap( 0 );
	m_staticText1801317202731->SetMinSize( wxSize( 150,-1 ) );

	bSizer18->Add( m_staticText1801317202731, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_embedding_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_embedding_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer18->Add( m_embedding_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer18, 0, wxALL, 5 );

	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText21 = new wxStaticText( m_path_panel, wxID_ANY, wxT("TAESD path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	m_staticText21->SetMinSize( wxSize( 150,-1 ) );

	bSizer23->Add( m_staticText21, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_taesd_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_taesd_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer23->Add( m_taesd_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton1 = new wxBitmapButton( m_path_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );

	m_bpButton1->SetBitmap( interrogation_png_to_wx_bitmap() );
	m_bpButton1->SetToolTip( wxT("TAESD models you can download from here: https://github.com/madebyollin/taesd/tree/main") );

	bSizer23->Add( m_bpButton1, 0, wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer23, 0, wxALL, 5 );

	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText23 = new wxStaticText( m_path_panel, wxID_ANY, wxT("Controlnet path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	m_staticText23->SetMinSize( wxSize( 150,-1 ) );

	bSizer35->Add( m_staticText23, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_controlnet_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_controlnet_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer35->Add( m_controlnet_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer35, 0, wxALL, 5 );

	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText77 = new wxStaticText( m_path_panel, wxID_ANY, wxT("ESRGAN path"), wxDefaultPosition, wxSize( 150,-1 ), 0 );
	m_staticText77->Wrap( -1 );
	bSizer81->Add( m_staticText77, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_esrgan_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL );
	m_esrgan_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer81->Add( m_esrgan_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer81, 0, wxALL, 5 );

	m_staticline7 = new wxStaticLine( m_path_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	sizer2011->Add( m_staticline7, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText19 = new wxStaticText( m_path_panel, wxID_ANY, wxT("Presets path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	m_staticText19->SetMinSize( wxSize( 150,-1 ) );

	bSizer20->Add( m_staticText19, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_presets_dir = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_SMALL );
	m_presets_dir->SetMinSize( wxSize( 200,-1 ) );

	bSizer20->Add( m_presets_dir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer20, 0, wxALL, 5 );

	m_staticLine223 = new wxStaticLine( m_path_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	sizer2011->Add( m_staticLine223, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText180131720 = new wxStaticText( m_path_panel, wxID_ANY, wxT("Images output"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText180131720->Wrap( 0 );
	m_staticText180131720->SetMinSize( wxSize( 150,-1 ) );

	bSizer21->Add( m_staticText180131720, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_images_output = new wxDirPickerCtrl( m_path_panel, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_SMALL|wxDIRP_DEFAULT_STYLE );
	m_images_output->SetMinSize( wxSize( 200,-1 ) );

	bSizer21->Add( m_images_output, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2011->Add( bSizer21, 0, wxALL, 5 );


	m_path_panel->SetSizer( sizer2011 );
	m_path_panel->Layout();
	sizer2011->Fit( m_path_panel );
	m_notebook1696->AddPage( m_path_panel, wxT("Paths"), false );
	m_settings = new wxPanel( m_notebook1696, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizer2017;
	sizer2017 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* sizer2018;
	sizer2018 = new wxGridSizer( 0, 2, 0, 0 );

	m_keep_model_in_memory = new wxCheckBox( m_settings, wxID_ANY, wxT("Keep model in memory"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_keep_model_in_memory->SetValue(true);
	m_keep_model_in_memory->Enable( false );
	m_keep_model_in_memory->SetMinSize( wxSize( 230,-1 ) );

	sizer2018->Add( m_keep_model_in_memory, 0, wxALIGN_LEFT|wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2017->Add( sizer2018, 0, wxALL, 5 );

	wxGridSizer* sizer2019;
	sizer2019 = new wxGridSizer( 0, 2, 0, 0 );

	m_save_all_image = new wxCheckBox( m_settings, wxID_ANY, wxT("Save all images (intermediat images)"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_save_all_image->SetValue(true);
	m_save_all_image->Enable( false );
	m_save_all_image->SetMinSize( wxSize( 230,-1 ) );

	sizer2019->Add( m_save_all_image, 0, wxALIGN_LEFT|wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2017->Add( sizer2019, 0, wxALL, 5 );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText16 = new wxStaticText( m_settings, wxID_ANY, wxT("Output images format"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	m_staticText16->SetMinSize( wxSize( 230,-1 ) );

	bSizer10->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxString m_image_typeChoices[] = { wxT("JPG"), wxT("PNG") };
	int m_image_typeNChoices = sizeof( m_image_typeChoices ) / sizeof( wxString );
	m_image_type = new wxChoice( m_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_image_typeNChoices, m_image_typeChoices, 0 );
	m_image_type->SetSelection( 0 );
	bSizer10->Add( m_image_type, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_image_quality = new wxSlider( m_settings, wxID_ANY, 95, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	m_image_quality->SetToolTip( wxT("Image output quality, default: 90%") );

	bSizer10->Add( m_image_quality, 0, wxALL, 5 );

	m_image_quality_spin = new wxSpinCtrl( m_settings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 95 );
	bSizer10->Add( m_image_quality_spin, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2017->Add( bSizer10, 0, wxALL, 5 );

	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText191 = new wxStaticText( m_settings, wxID_ANY, wxT("Number of CPU cores"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText191->Wrap( -1 );
	m_staticText191->SetMinSize( wxSize( 230,-1 ) );

	bSizer22->Add( m_staticText191, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_threads = new wxSpinCtrl( m_settings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -1, 100, 2 );
	bSizer22->Add( m_threads, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sizer2017->Add( bSizer22, 0, wxALL, 5 );


	m_settings->SetSizer( sizer2017 );
	m_settings->Layout();
	sizer2017->Fit( m_settings );
	m_notebook1696->AddPage( m_settings, wxT("Diffusion"), false );
	m_settings_ui = new wxPanel( m_notebook1696, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );

	m_show_notifications = new wxCheckBox( m_settings_ui, wxID_ANY, wxT("Show notifications"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_show_notifications->SetValue(true);
	bSizer31->Add( m_show_notifications, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText60 = new wxStaticText( m_settings_ui, wxID_ANY, wxT("Notification timeout:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText60->Wrap( -1 );
	bSizer31->Add( m_staticText60, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_notification_timeout = new wxSpinCtrl( m_settings_ui, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 30, 120, 60 );
	m_notification_timeout->SetToolTip( wxT("The timeout is depends on the OS and notification type") );

	bSizer31->Add( m_notification_timeout, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer30->Add( bSizer31, 1, wxALL, 5 );


	m_settings_ui->SetSizer( bSizer30 );
	m_settings_ui->Layout();
	bSizer30->Fit( m_settings_ui );
	m_notebook1696->AddPage( m_settings_ui, wxT("GUI"), false );

	sizer2010->Add( m_notebook1696, 1, wxALL|wxEXPAND, 5 );

	m_save = new wxButton( this, wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 );

	m_save->SetBitmap( disk_png_to_wx_bitmap() );
	sizer2010->Add( m_save, 0, wxALL, 5 );


	this->SetSizer( sizer2010 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_image_quality->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality_spin->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( Settings::OnImgQualitySpin ), NULL, this );
	m_show_notifications->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( Settings::onShowNotificationCheck ), NULL, this );
	m_save->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::onSave ), NULL, this );
}

Settings::~Settings()
{
	// Disconnect Events
	m_image_quality->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( Settings::OnImgQualityScroll ), NULL, this );
	m_image_quality_spin->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( Settings::OnImgQualitySpin ), NULL, this );
	m_show_notifications->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( Settings::onShowNotificationCheck ), NULL, this );
	m_save->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::onSave ), NULL, this );

}

ImageViewer::ImageViewer( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxFrame( parent, id, title, pos, size, style, name )
{
	this->SetSizeHints( wxSize( 512,512 ), wxDefaultSize );

	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );

	m_panel9 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel9->SetMinSize( wxSize( 500,-1 ) );

	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer( wxVERTICAL );

	m_scrolledWindow1 = new wxScrolledWindow( m_panel9, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow1->SetScrollRate( 5, 5 );
	m_scrolledWindow1->SetMinSize( wxSize( 600,600 ) );

	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxVERTICAL );

	m_bitmap6 = new wxStaticBitmap( m_scrolledWindow1, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 512,512 ), 0 );
	bSizer40->Add( m_bitmap6, 1, wxALL|wxEXPAND, 5 );


	m_scrolledWindow1->SetSizer( bSizer40 );
	m_scrolledWindow1->Layout();
	bSizer40->Fit( m_scrolledWindow1 );
	bSizer44->Add( m_scrolledWindow1, 1, wxEXPAND | wxALL, 5 );


	m_panel9->SetSizer( bSizer44 );
	m_panel9->Layout();
	bSizer44->Fit( m_panel9 );
	bSizer41->Add( m_panel9, 1, wxEXPAND | wxALL, 5 );

	m_panel10 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel10->SetMinSize( wxSize( 300,-1 ) );

	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxVERTICAL );

	m_splitter5 = new wxSplitterWindow( m_panel10, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_3DBORDER|wxSP_BORDER|wxSP_LIVE_UPDATE|wxBORDER_THEME );
	m_splitter5->Connect( wxEVT_IDLE, wxIdleEventHandler( ImageViewer::m_splitter5OnIdle ), NULL, this );

	m_panel17 = new wxPanel( m_splitter5, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	wxBoxSizer* bSizer52;
	bSizer52 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer54;
	bSizer54 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText25 = new wxStaticText( m_panel17, wxID_ANY, wxT("ID"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText25->Wrap( -1 );
	bSizer54->Add( m_staticText25, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_id = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_id->Wrap( -1 );
	bSizer54->Add( m_static_id, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer54, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer541;
	bSizer541 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText251 = new wxStaticText( m_panel17, wxID_ANY, wxT("Type"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText251->Wrap( -1 );
	bSizer541->Add( m_staticText251, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_type = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_type->Wrap( -1 );
	bSizer541->Add( m_static_type, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer541, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer542;
	bSizer542 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText252 = new wxStaticText( m_panel17, wxID_ANY, wxT("Model"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText252->Wrap( -1 );
	bSizer542->Add( m_staticText252, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_model = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_model->Wrap( -1 );
	bSizer542->Add( m_static_model, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer542, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer543;
	bSizer543 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText253 = new wxStaticText( m_panel17, wxID_ANY, wxT("Dimensions"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText253->Wrap( -1 );
	bSizer543->Add( m_staticText253, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_resolution = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_resolution->Wrap( -1 );
	bSizer543->Add( m_static_resolution, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer543, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer546;
	bSizer546 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText256 = new wxStaticText( m_panel17, wxID_ANY, wxT("CFG Scale"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText256->Wrap( -1 );
	bSizer546->Add( m_staticText256, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_cfg_scale = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_cfg_scale->Wrap( -1 );
	bSizer546->Add( m_static_cfg_scale, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer546, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer547;
	bSizer547 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText257 = new wxStaticText( m_panel17, wxID_ANY, wxT("Clip skip"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText257->Wrap( -1 );
	bSizer547->Add( m_staticText257, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_clip_skip = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_clip_skip->Wrap( -1 );
	bSizer547->Add( m_static_clip_skip, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer547, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer548;
	bSizer548 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText258 = new wxStaticText( m_panel17, wxID_ANY, wxT("Seed"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText258->Wrap( -1 );
	bSizer548->Add( m_staticText258, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_seed = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_seed->Wrap( -1 );
	bSizer548->Add( m_static_seed, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer548, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer549;
	bSizer549 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText259 = new wxStaticText( m_panel17, wxID_ANY, wxT("Steps"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText259->Wrap( -1 );
	bSizer549->Add( m_staticText259, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_steps = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_steps->Wrap( -1 );
	bSizer549->Add( m_static_steps, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer549, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer5410;
	bSizer5410 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2510 = new wxStaticText( m_panel17, wxID_ANY, wxT("Sampler"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText2510->Wrap( -1 );
	bSizer5410->Add( m_staticText2510, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_sampler = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_sampler->Wrap( -1 );
	bSizer5410->Add( m_static_sampler, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer5410, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer5411;
	bSizer5411 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2511 = new wxStaticText( m_panel17, wxID_ANY, wxT("Sheduler"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText2511->Wrap( -1 );
	bSizer5411->Add( m_staticText2511, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_sheduler = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_sheduler->Wrap( -1 );
	bSizer5411->Add( m_static_sheduler, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer5411, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer54111;
	bSizer54111 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText25111 = new wxStaticText( m_panel17, wxID_ANY, wxT("Started at"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText25111->Wrap( -1 );
	bSizer54111->Add( m_staticText25111, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_started = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_started->Wrap( -1 );
	bSizer54111->Add( m_static_started, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer54111, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer54112;
	bSizer54112 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText25112 = new wxStaticText( m_panel17, wxID_ANY, wxT("Finished at"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText25112->Wrap( -1 );
	bSizer54112->Add( m_staticText25112, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_finished = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_finished->Wrap( -1 );
	bSizer54112->Add( m_static_finished, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer54112, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer5412;
	bSizer5412 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2512 = new wxStaticText( m_panel17, wxID_ANY, wxT("Batch"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText2512->Wrap( -1 );
	bSizer5412->Add( m_staticText2512, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_static_batch = new wxStaticText( m_panel17, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_batch->Wrap( -1 );
	bSizer5412->Add( m_static_batch, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer52->Add( bSizer5412, 1, wxEXPAND, 5 );

	m_staticline7 = new wxStaticLine( m_panel17, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer52->Add( m_staticline7, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer544;
	bSizer544 = new wxBoxSizer( wxVERTICAL );

	m_staticText254 = new wxStaticText( m_panel17, wxID_ANY, wxT("Prompt"), wxDefaultPosition, wxSize( 80,-1 ), wxALIGN_CENTER_HORIZONTAL );
	m_staticText254->Wrap( -1 );
	bSizer544->Add( m_staticText254, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_static_prompt = new wxTextCtrl( m_panel17, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP|wxTE_MULTILINE|wxTE_NO_VSCROLL|wxTE_READONLY|wxTE_WORDWRAP|wxBORDER_NONE|wxTRANSPARENT_WINDOW );
	#ifdef __WXGTK__
	if ( !m_static_prompt->HasFlag( wxTE_MULTILINE ) )
	{
	m_static_prompt->SetMaxLength( 255 );
	}
	#else
	m_static_prompt->SetMaxLength( 255 );
	#endif
	m_static_prompt->SetExtraStyle( wxWS_EX_TRANSIENT );
	m_static_prompt->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	m_static_prompt->SetMinSize( wxSize( -1,80 ) );

	bSizer544->Add( m_static_prompt, 1, wxALL|wxEXPAND, 5 );


	bSizer52->Add( bSizer544, 1, wxEXPAND, 5 );

	m_staticline71 = new wxStaticLine( m_panel17, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer52->Add( m_staticline71, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer545;
	bSizer545 = new wxBoxSizer( wxVERTICAL );

	m_staticText255 = new wxStaticText( m_panel17, wxID_ANY, wxT("Neg. prompt"), wxDefaultPosition, wxSize( 80,-1 ), wxALIGN_CENTER_HORIZONTAL );
	m_staticText255->Wrap( -1 );
	bSizer545->Add( m_staticText255, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_static_negative_prompt = new wxTextCtrl( m_panel17, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP|wxTE_MULTILINE|wxTE_NO_VSCROLL|wxTE_READONLY|wxTE_WORDWRAP|wxBORDER_NONE|wxTRANSPARENT_WINDOW );
	#ifdef __WXGTK__
	if ( !m_static_negative_prompt->HasFlag( wxTE_MULTILINE ) )
	{
	m_static_negative_prompt->SetMaxLength( 255 );
	}
	#else
	m_static_negative_prompt->SetMaxLength( 255 );
	#endif
	m_static_negative_prompt->SetExtraStyle( wxWS_EX_TRANSIENT );
	m_static_negative_prompt->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	m_static_negative_prompt->SetMinSize( wxSize( -1,80 ) );

	bSizer545->Add( m_static_negative_prompt, 1, wxALL|wxEXPAND, 5 );


	bSizer52->Add( bSizer545, 1, wxEXPAND, 5 );


	m_panel17->SetSizer( bSizer52 );
	m_panel17->Layout();
	bSizer52->Fit( m_panel17 );
	m_panel18 = new wxPanel( m_splitter5, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	wxBoxSizer* bSizer53;
	bSizer53 = new wxBoxSizer( wxVERTICAL );

	m_scrolledWindow2 = new wxScrolledWindow( m_panel18, wxID_ANY, wxDefaultPosition, wxSize( -1,200 ), wxBORDER_SIMPLE|wxCLIP_CHILDREN|wxFULL_REPAINT_ON_RESIZE|wxHSCROLL );
	m_scrolledWindow2->SetScrollRate( 5, 5 );
	thumbnails_container = new wxGridSizer( 0, 6, 0, 0 );


	m_scrolledWindow2->SetSizer( thumbnails_container );
	m_scrolledWindow2->Layout();
	bSizer53->Add( m_scrolledWindow2, 1, wxALL|wxEXPAND, 5 );


	m_panel18->SetSizer( bSizer53 );
	m_panel18->Layout();
	bSizer53->Fit( m_panel18 );
	m_splitter5->SplitHorizontally( m_panel17, m_panel18, 0 );
	bSizer43->Add( m_splitter5, 1, wxEXPAND, 5 );


	m_panel10->SetSizer( bSizer43 );
	m_panel10->Layout();
	bSizer43->Fit( m_panel10 );
	bSizer41->Add( m_panel10, 1, wxEXPAND | wxALL, 5 );


	bSizer39->Add( bSizer41, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer39 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_bitmap6->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( ImageViewer::onImageViewClick ), NULL, this );
}

ImageViewer::~ImageViewer()
{
	// Disconnect Events
	m_bitmap6->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( ImageViewer::onImageViewClick ), NULL, this );

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

Modelinfo::Modelinfo( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer47;
	bSizer47 = new wxBoxSizer( wxVERTICAL );

	m_splitter4 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_3DBORDER|wxSP_3DSASH|wxSP_BORDER|wxSP_LIVE_UPDATE );
	m_splitter4->Connect( wxEVT_IDLE, wxIdleEventHandler( Modelinfo::m_splitter4OnIdle ), NULL, this );
	m_splitter4->SetMinimumPaneSize( 300 );

	m_panel13 = new wxPanel( m_splitter4, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer48;
	bSizer48 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer64;
	bSizer64 = new wxBoxSizer( wxHORIZONTAL );


	bSizer48->Add( bSizer64, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer65;
	bSizer65 = new wxBoxSizer( wxHORIZONTAL );

	m_static_model_name = new wxStaticText( m_panel13, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_model_name->Wrap( -1 );
	bSizer65->Add( m_static_model_name, 1, wxALL, 5 );

	m_static_model_type = new wxStaticText( m_panel13, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_model_type->Wrap( -1 );
	bSizer65->Add( m_static_model_type, 1, wxALL, 5 );


	bSizer48->Add( bSizer65, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer651;
	bSizer651 = new wxBoxSizer( wxHORIZONTAL );

	m_static_model_created_at = new wxStaticText( m_panel13, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_model_created_at->Wrap( -1 );
	bSizer651->Add( m_static_model_created_at, 1, wxALL, 5 );

	m_static_model_nsfw = new wxStaticText( m_panel13, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_model_nsfw->Wrap( -1 );
	bSizer651->Add( m_static_model_nsfw, 1, wxALL, 5 );


	bSizer48->Add( bSizer651, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer6511;
	bSizer6511 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText66 = new wxStaticText( m_panel13, wxID_ANY, wxT("Base model"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_staticText66->Wrap( -1 );
	bSizer6511->Add( m_staticText66, 0, wxALL, 5 );

	m_static_model_basemodel = new wxStaticText( m_panel13, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_static_model_basemodel->Wrap( -1 );
	bSizer6511->Add( m_static_model_basemodel, 1, wxALL, 5 );

	m_static_model_basemodeltype = new wxStaticText( m_panel13, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_static_model_basemodeltype->Wrap( -1 );
	bSizer6511->Add( m_static_model_basemodeltype, 1, wxALL, 5 );


	bSizer48->Add( bSizer6511, 0, wxEXPAND, 5 );

	m_htmlWin1 = new wxHtmlWindow( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_NO_SELECTION|wxHW_SCROLLBAR_AUTO );
	bSizer48->Add( m_htmlWin1, 1, wxALL|wxEXPAND, 1 );


	m_panel13->SetSizer( bSizer48 );
	m_panel13->Layout();
	bSizer48->Fit( m_panel13 );
	m_scrolledWindow3 = new wxScrolledWindow( m_splitter4, wxID_ANY, wxDefaultPosition, wxSize( 512,-1 ), wxHSCROLL|wxVSCROLL );
	m_scrolledWindow3->SetScrollRate( 5, 5 );
	poster_holder = new wxBoxSizer( wxVERTICAL );

	poster_holder->SetMinSize( wxSize( 512,-1 ) );

	m_scrolledWindow3->SetSizer( poster_holder );
	m_scrolledWindow3->Layout();
	m_splitter4->SplitVertically( m_panel13, m_scrolledWindow3, 0 );
	bSizer47->Add( m_splitter4, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer47 );
	this->Layout();

	this->Centre( wxBOTH );
}

Modelinfo::~Modelinfo()
{
}
