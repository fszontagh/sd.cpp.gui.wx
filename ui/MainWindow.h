///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.0.0-0-g0efcecf)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/cshelp.h>
#include <wx/bmpbuttn.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/statbmp.h>
#include <wx/filepicker.h>
#include <wx/srchctrl.h>
#include <wx/hyperlink.h>
#include <wx/html/htmlwin.h>
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/statline.h>
#include <wx/slider.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class UI
///////////////////////////////////////////////////////////////////////////////
class UI : public wxFrame
{
	private:

	protected:
		wxPanel* m_panel14;
		wxBitmapButton* m_settings;
		wxBitmapButton* m_refrersh;
		wxStaticText* m_staticText160;
		wxChoice* m_model;
		wxStaticText* m_staticText72;
		wxChoice* m_type;
		wxStaticText* m_staticText161;
		wxChoice* m_vae;
		wxCheckBox* m_vae_tiling;
		wxCheckBox* m_vae_decode_only;
		wxStaticText* m_staticText231;
		wxSpinCtrlDouble* m_cfg;
		wxStaticText* m_staticText234;
		wxSpinCtrl* m_clip_skip;
		wxStaticText* m_staticText20;
		wxChoice* m_taesd;
		wxStaticText* m_staticText15;
		wxSpinCtrl* m_batch_count;
		wxStaticText* m_staticText163;
		wxChoice* m_sampler;
		wxStaticText* m_staticText17;
		wxButton* m_save_preset;
		wxButton* m_load_preset;
		wxChoice* m_preset_list;
		wxBitmapButton* m_delete_preset;
		wxStaticText* m_staticText233;
		wxSpinCtrl* m_seed;
		wxBitmapButton* m_random_seed;
		wxStaticText* m_staticText14;
		wxSpinCtrl* m_steps;
		wxStaticText* m_staticText237;
		wxSpinCtrl* m_width;
		wxStaticText* m_staticText239;
		wxSpinCtrl* m_height;
		wxButton* m_button7;
		wxNotebook* m_notebook1302;
		wxPanel* m_jobs_panel;
		wxButton* m_start_jobs;
		wxButton* m_pause_jobs;
		wxButton* m_delete_all_jobs;
		wxDataViewListCtrl* m_joblist;
		wxDataViewListCtrl* m_joblist_item_details;
		wxDataViewColumn* m_dataViewListColumn1;
		wxDataViewColumn* m_dataViewListColumn2;
		wxListCtrl* m_job_details_imagelist;
		wxPanel* m_text2img_panel;
		wxTextCtrl* m_prompt;
		wxTextCtrl* m_neg_prompt;
		wxStaticBitmap* m_controlnetImagePreview;
		wxButton* m_generate2;
		wxFilePickerCtrl* m_controlnetImageOpen;
		wxChoice* m_controlnetModels;
		wxSpinCtrlDouble* m_controlnetStrength;
		wxButton* m_controlnetImagePreviewButton;
		wxBitmapButton* m_controlnetImageDelete;
		wxPanel* m_image2image_panel;
		wxFilePickerCtrl* m_open_image;
		wxButton* m_img2im_preview_img;
		wxBitmapButton* m_delete_initial_img;
		wxTextCtrl* m_prompt2;
		wxTextCtrl* m_neg_prompt2;
		wxButton* m_generate1;
		wxStaticText* m_staticText24;
		wxSpinCtrlDouble* m_strength;
		wxStaticBitmap* m_img2img_preview;
		wxPanel* m_upscaler;
		wxStaticText* m_staticText62;
		wxFilePickerCtrl* m_upscaler_filepicker;
		wxBitmapButton* m_delete_upscale_image;
		wxStaticText* m_staticText61;
		wxChoice* m_upscaler_model;
		wxStaticText* m_staticText63;
		wxStaticText* m_static_upscaler_width;
		wxStaticText* m_staticText65;
		wxStaticText* m_static_upscaler_height;
		wxStaticText* m_staticText631;
		wxStaticText* m_static_upscaler_target_width;
		wxStaticText* m_staticText651;
		wxStaticText* m_static_upscaler_target_height;
		wxStaticText* m_staticText67;
		wxSpinCtrl* m_upscaler_factor;
		wxStaticBitmap* m_upscaler_source_image;
		wxButton* m_generate_upscaler;
		wxCheckBox* m_keep_upscaler_in_memory;
		wxCheckBox* m_keep_other_models_in_memory;
		wxPanel* m_models_panel;
		wxCheckBox* m_checkbox_lora_filter;
		wxCheckBox* m_checkbox_filter_checkpoints;
		wxCheckBox* m_checkbox_filter_embeddings;
		wxSearchCtrl* m_modellist_filter;
		wxDataViewListCtrl* m_data_model_list;
		wxDataViewColumn* m_dataViewListColumn3;
		wxDataViewColumn* m_dataViewListColumn4;
		wxDataViewColumn* m_dataViewListColumn5;
		wxDataViewColumn* m_dataViewListColumn6;
		wxDataViewColumn* m_dataViewListColumn8;
		wxDataViewColumn* m_dataViewListColumn7;
		wxHyperlinkCtrl* m_data_model_url;
		wxDataViewListCtrl* m_model_details;
		wxDataViewColumn* m_dataViewListColumn11;
		wxDataViewColumn* m_dataViewListColumn21;
		wxHtmlWindow* m_model_details_description;
		wxListCtrl* m_model_details_imagelist;
		wxTextCtrl* logs;
		wxStatusBar* m_statusBar166;

		// Virtual event handlers, override them in your derived class
		virtual void onSettings( wxCommandEvent& event ) { event.Skip(); }
		virtual void onModelsRefresh( wxCommandEvent& event ) { event.Skip(); }
		virtual void onModelSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onTypeSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVaeSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSamplerSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSavePreset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onLoadPreset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSelectPreset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDeletePreset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onRandomGenerateButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void onResolutionSwap( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_notebook1302OnNotebookPageChanged( wxNotebookEvent& event ) { event.Skip(); }
		virtual void onJobsStart( wxCommandEvent& event ) { event.Skip(); }
		virtual void onJobsPause( wxCommandEvent& event ) { event.Skip(); }
		virtual void onJobsDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnJobListItemActivated( wxDataViewEvent& event ) { event.Skip(); }
		virtual void onContextMenu( wxDataViewEvent& event ) { event.Skip(); }
		virtual void OnJobListItemSelection( wxDataViewEvent& event ) { event.Skip(); }
		virtual void OnJobDetailsImagelistItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void onTxt2ImgFileDrop( wxDropFilesEvent& event ) { event.Skip(); }
		virtual void OnPromptText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNegPromptText( wxCommandEvent& event ) { event.Skip(); }
		virtual void onGenerate( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnControlnetImageOpen( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnControlnetImagePreviewButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnControlnetImageDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void Onimg2imgDropFile( wxDropFilesEvent& event ) { event.Skip(); }
		virtual void OnImageOpenFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnImg2ImgPreviewButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteInitialImage( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpscalerDropFile( wxDropFilesEvent& event ) { event.Skip(); }
		virtual void OnImageOpenFilePickerChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnDeleteUpscaleImage( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpscalerModelSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpscalerFactorChange( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnCheckboxLoraFilter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckboxCheckpointFilter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnModellistFilterKeyUp( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnDataModelActivated( wxDataViewEvent& event ) { event.Skip(); }
		virtual void OnDataModelSelected( wxDataViewEvent& event ) { event.Skip(); }


	public:

		UI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("sd.cpp.gui"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxBORDER_DEFAULT, const wxString& name = wxT("sd.cpp.gui") );

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
		wxStaticText* m_staticText77;
		wxDirPickerCtrl* m_esrgan_dir;
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
		wxChoice* m_image_type;
		wxSlider* m_image_quality;
		wxSpinCtrl* m_image_quality_spin;
		wxStaticText* m_staticText191;
		wxSpinCtrl* m_threads;
		wxPanel* m_settings_ui;
		wxCheckBox* m_show_notifications;
		wxStaticText* m_staticText60;
		wxSpinCtrl* m_notification_timeout;
		wxButton* m_save;

		// Virtual event handlers, override them in your derived class
		virtual void OnImgQualityScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnImgQualitySpin( wxSpinEvent& event ) { event.Skip(); }
		virtual void onShowNotificationCheck( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSave( wxCommandEvent& event ) { event.Skip(); }


	public:

		Settings( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 600,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxFRAME_NO_TASKBAR|wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL|wxBORDER_THEME, const wxString& name = wxT("sd.cpp.gui.settings") );

		~Settings();

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

		ImageDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = wxT("sd.cpp.gui.imagedialog") );

		~ImageDialog();

};

