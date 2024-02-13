///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.0.0-0-g0efcecf)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/statbmp.h>
#include <wx/filepicker.h>
#include <wx/bmpbuttn.h>
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class UI
///////////////////////////////////////////////////////////////////////////////
class UI : public wxFrame
{
	private:

	protected:
		wxPanel* m_all_panel;
		wxButton* m_settings;
		wxButton* m_refresh;
		wxStaticText* m_staticText160;
		wxChoice* m_model;
		wxChoice* m_type;
		wxStaticText* m_staticText161;
		wxChoice* m_vae;
		wxStaticText* m_staticText233;
		wxSpinCtrl* m_seed;
		wxButton* m_random_seed;
		wxStaticText* m_staticText14;
		wxSpinCtrl* m_steps;
		wxStaticLine* m_staticLine236;
		wxStaticText* m_staticText237;
		wxSpinCtrl* m_width;
		wxStaticText* m_staticText239;
		wxSpinCtrl* m_height;
		wxButton* m_button7;
		wxStaticText* m_staticText231;
		wxSpinCtrlDouble* m_cfg;
		wxStaticText* m_staticText234;
		wxSpinCtrl* m_clip_skip;
		wxStaticLine* m_staticline51;
		wxCheckBox* m_vae_tiling;
		wxCheckBox* m_vae_decode_only;
		wxStaticLine* m_staticline6;
		wxStaticText* m_staticText20;
		wxChoice* m_taesd;
		wxNotebook* m_notebook1302;
		wxPanel* m_jobs_panel;
		wxButton* m_start_jobs;
		wxButton* m_pause_jobs;
		wxButton* m_delete_all_jobs;
		wxDataViewListCtrl* m_joblist;
		wxPanel* m_text2img_panel;
		wxTextCtrl* m_prompt;
		wxTextCtrl* m_neg_prompt;
		wxStaticBitmap* m_controlnetImagePreview;
		wxButton* m_generate2;
		wxStaticText* m_staticText22;
		wxFilePickerCtrl* m_controlnetImageOpen;
		wxSpinCtrlDouble* m_controlnetStrength;
		wxButton* m_controlnetImagePreviewButton;
		wxBitmapButton* m_controlnetImageDelete;
		wxChoice* m_controlnetModels;
		wxPanel* m_image2image_panel;
		wxFilePickerCtrl* m_open_image;
		wxButton* m_img2im_preview_img;
		wxButton* m_delete_initial_img;
		wxTextCtrl* m_prompt2;
		wxTextCtrl* m_neg_prompt2;
		wxButton* m_generate1;
		wxStaticText* m_staticText24;
		wxSpinCtrlDouble* m_strength;
		wxStaticBitmap* m_img2img_preview;
		wxPanel* m_models_panel;
		wxDataViewListCtrl* m_data_model_list;
		wxStaticText* m_staticText15;
		wxSpinCtrl* m_batch_count;
		wxStaticText* m_staticText163;
		wxChoice* m_sampler;
		wxStaticLine* m_staticline5;
		wxStaticText* m_staticText17;
		wxButton* m_save_preset;
		wxButton* m_load_preset;
		wxChoice* m_preset_list;
		wxButton* m_delete_preset;
		wxTextCtrl* logs;
		wxStatusBar* m_statusBar166;

		// Virtual event handlers, override them in your derived class
		virtual void onSettings( wxCommandEvent& event ) { event.Skip(); }
		virtual void onModelsRefresh( wxCommandEvent& event ) { event.Skip(); }
		virtual void onModelSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onTypeSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVaeSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onRandomGenerateButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void onResolutionSwap( wxCommandEvent& event ) { event.Skip(); }
		virtual void onJobsStart( wxCommandEvent& event ) { event.Skip(); }
		virtual void onJobsPause( wxCommandEvent& event ) { event.Skip(); }
		virtual void onJobsDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void onJoblistItemActivated( wxDataViewEvent& event ) { event.Skip(); }
		virtual void onContextMenu( wxDataViewEvent& event ) { event.Skip(); }
		virtual void onJoblistSelectionChanged( wxDataViewEvent& event ) { event.Skip(); }
		virtual void onGenerate( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnControlnetImageOpen( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnControlnetImagePreviewButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnControlnetImageDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void Onimg2imgDropFile( wxDropFilesEvent& event ) { event.Skip(); }
		virtual void OnImageOpenFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnImg2ImgPreviewButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteInitialImage( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSamplerSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSavePreset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onLoadPreset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSelectPreset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDeletePreset( wxCommandEvent& event ) { event.Skip(); }


	public:

		UI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("sd.cpp.gui"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxCAPTION|wxCLOSE_BOX|wxICONIZE|wxMINIMIZE_BOX|wxSYSTEM_MENU|wxBORDER_THEME|wxCLIP_CHILDREN );

		~UI();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Settings
///////////////////////////////////////////////////////////////////////////////
class Settings : public wxFrame
{
	private:

	protected:
		wxNotebook* m_notebook1696;
		wxPanel* m_path_panel;
		wxStaticText* m_staticText172410;
		wxDirPickerCtrl* m_model_dir;
		wxStaticText* m_staticText18013172027;
		wxDirPickerCtrl* m_lora_dir;
		wxStaticText* m_staticText18013;
		wxDirPickerCtrl* m_vae_dir;
		wxStaticText* m_staticText1801317202731;
		wxDirPickerCtrl* m_embedding_dir;
		wxStaticText* m_staticText21;
		wxDirPickerCtrl* m_taesd_dir;
		wxBitmapButton* m_bpButton1;
		wxStaticText* m_staticText23;
		wxDirPickerCtrl* m_controlnet_dir;
		wxStaticLine* m_staticline7;
		wxStaticText* m_staticText19;
		wxDirPickerCtrl* m_presets_dir;
		wxStaticLine* m_staticLine223;
		wxStaticText* m_staticText180131720;
		wxDirPickerCtrl* m_images_output;
		wxPanel* m_settings;
		wxCheckBox* m_keep_model_in_memory;
		wxCheckBox* m_save_all_image;
		wxStaticText* m_staticText16;
		wxChoice* m_choice4;
		wxStaticText* m_staticText191;
		wxSpinCtrl* m_threads;
		wxPanel* m_settings_ui;
		wxCheckBox* m_checkBox5;
		wxButton* m_save;

		// Virtual event handlers, override them in your derived class
		virtual void onSave( wxCommandEvent& event ) { event.Skip(); }


	public:

		Settings( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 600,400 ), long style = wxFRAME_NO_TASKBAR|wxFRAME_FLOAT_ON_PARENT|wxCAPTION|wxCLOSE_BOX|wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL|wxBORDER_THEME );

		~Settings();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ImageViewer
///////////////////////////////////////////////////////////////////////////////
class ImageViewer : public wxFrame
{
	private:

	protected:

	public:
		wxStaticBitmap* m_bitmap;
		wxTextCtrl* m_textCtrl4;

		ImageViewer( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~ImageViewer();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Image
///////////////////////////////////////////////////////////////////////////////
class Image : public wxPanel
{
	private:

	protected:

	public:
		wxStaticBitmap* m_bitmap3;

		Image( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~Image();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ImageDialog
///////////////////////////////////////////////////////////////////////////////
class ImageDialog : public wxDialog
{
	private:

	protected:

	public:
		wxStaticBitmap* m_bitmap;

		ImageDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );

		~ImageDialog();

};

