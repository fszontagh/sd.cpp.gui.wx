///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/cshelp.h>
#include <wx/intl.h>
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
#include <wx/tglbtn.h>
#include <wx/statline.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/gauge.h>
#include <wx/panel.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/scrolwin.h>
#include <wx/dataview.h>
#include <wx/splitter.h>
#include <wx/statbmp.h>
#include <wx/textctrl.h>
#include <wx/filepicker.h>
#include <wx/notebook.h>
#include <wx/html/htmlwin.h>
#include <wx/srchctrl.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/slider.h>
#include <wx/dialog.h>
#include <wx/radiobox.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class mainUI
///////////////////////////////////////////////////////////////////////////////
class mainUI : public wxFrame
{
	private:

	protected:
		wxPanel* m_panel10;
		wxBitmapButton* m_settings;
		wxBitmapButton* m_refrersh;
		wxBitmapButton* m_about;
		wxBitmapButton* m_civitai;
		wxBitmapToggleButton* m_showWidget;
		wxBitmapButton* m_stop_background_process;
		wxStaticLine* m_staticline5;
		wxStaticText* m_staticText160;
		wxChoice* m_model;
		wxStaticText* m_staticText72;
		wxChoice* m_type;
		wxStaticText* m_currentStatus;
		wxGauge* m_currentProgress;
		wxPanel* m_panel12;
		wxScrolledWindow* m_scrolledWindow1;
		wxStaticText* m_staticText161;
		wxChoice* m_vae;
		wxCheckBox* vaeOnCpu;
		wxCheckBox* m_vae_tiling;
		wxCheckBox* m_vae_decode_only;
		wxStaticText* m_staticText231;
		wxSpinCtrlDouble* m_cfg;
		wxStaticText* m_staticText234;
		wxSpinCtrl* m_clip_skip;
		wxCheckBox* clipOnCpu;
		wxStaticText* m_staticText20;
		wxChoice* m_taesd;
		wxStaticText* m_staticText15;
		wxSpinCtrl* m_batch_count;
		wxStaticText* m_staticText163;
		wxChoice* m_sampler;
		wxStaticText* m_schedulertext;
		wxChoice* m_scheduler;
		wxStaticText* m_staticText233;
		wxSpinCtrl* m_seed;
		wxBitmapButton* m_random_seed;
		wxStaticText* m_staticText14;
		wxSpinCtrl* m_steps;
		wxStaticText* m_staticText237;
		wxSpinCtrl* m_width;
		wxStaticText* m_staticText239;
		wxSpinCtrl* m_height;
		wxBitmapButton* m_button7;
		wxStaticLine* m_staticline4;
		wxStaticText* m_staticText17;
		wxBitmapButton* m_save_preset;
		wxBitmapButton* m_load_preset;
		wxChoice* m_preset_list;
		wxBitmapButton* m_delete_preset;
		wxStaticText* m_staticText46;
		wxChoice* m_sd15Res;
		wxStaticText* m_staticText461;
		wxChoice* m_sdXlres;
		wxStaticText* m_staticText49;
		wxChoice* m_promptPresets;
		wxPanel* m_panel11;
		wxNotebook* m_notebook1302;
		wxPanel* m_jobs_panel;
		wxButton* m_start_jobs;
		wxButton* m_pause_jobs;
		wxButton* m_delete_all_jobs;
		wxStaticText* m_static_number_of_jobs;
		wxSplitterWindow* m_splitter2;
		wxPanel* m_panel14;
		wxDataViewListCtrl* m_joblist;
		wxDataViewColumn* m_dataViewListColumn32;
		wxDataViewColumn* m_dataViewListColumn321;
		wxDataViewColumn* m_dataViewListColumn3211;
		wxDataViewColumn* m_dataViewListColumn32111;
		wxDataViewColumn* m_dataViewListColumn321111;
		wxDataViewColumn* m_dataViewListColumn3211111;
		wxDataViewColumn* m_dataViewListColumn32111111;
		wxDataViewColumn* m_dataViewListColumn321111111;
		wxDataViewColumn* m_dataViewListColumn3211111111;
		wxDataViewColumn* m_dataViewListColumn32111111111;
		wxPanel* m_panel15;
		wxDataViewListCtrl* m_joblist_item_details;
		wxDataViewColumn* m_dataViewListColumn1;
		wxDataViewColumn* m_dataViewListColumn2;
		wxScrolledWindow* m_scrolledWindow41;
		wxStaticBitmap* m_bitmap6;
		wxPanel* m_text2img_panel;
		wxTextCtrl* m_prompt;
		wxTextCtrl* m_neg_prompt;
		wxBitmapButton* m_bpButton25;
		wxStaticText* m_staticText70;
		wxStaticText* m_staticText522;
		wxSpinCtrlDouble* slgScale;
		wxStaticText* m_staticText5221;
		wxTextCtrl* m_skipLayers;
		wxStaticText* m_staticText52211;
		wxSpinCtrlDouble* skipLayerStart;
		wxStaticText* m_staticText522111;
		wxSpinCtrlDouble* skipLayerEnd;
		wxStaticLine* m_staticline41;
		wxStaticText* m_staticText52111;
		wxFilePickerCtrl* m_filePickerDiffusionModel;
		wxCheckBox* diffusionFlashAttn;
		wxBitmapButton* m_cleanDiffusionModel;
		wxStaticText* m_staticText52;
		wxFilePickerCtrl* m_filePickerClipL;
		wxStaticText* m_staticText521;
		wxFilePickerCtrl* m_filePickerClipG;
		wxStaticText* m_staticText5211;
		wxFilePickerCtrl* m_filePickerT5XXL;
		wxNotebook* m_notebook3;
		wxPanel* m_diffusionPreviewTab;
		wxStaticBitmap* m_txt2txtDeiffusionPreview;
		wxPanel* m_controlnetPreviewTab;
		wxStaticBitmap* m_controlnetImagePreview;
		wxButton* m_generate2;
		wxFilePickerCtrl* m_controlnetImageOpen;
		wxCheckBox* cnOnCpu;
		wxChoice* m_controlnetModels;
		wxSpinCtrlDouble* m_controlnetStrength;
		wxButton* m_controlnetImagePreviewButton;
		wxBitmapButton* m_controlnetImageDelete;
		wxPanel* m_image2image_panel;
		wxTextCtrl* m_prompt2;
		wxTextCtrl* m_neg_prompt2;
		wxNotebook* m_notebook4;
		wxPanel* m_panel23;
		wxStaticBitmap* m_img2imgDiffusionPreview;
		wxPanel* m_panel25;
		wxPanel* m_panel26;
		wxStaticText* m_inpaintBrushSize;
		wxStaticText* m_inpaintZoom;
		wxStaticText* m_inpaintImageResolution;
		wxButton* m_inpaintSaveMask;
		wxButton* m_inpaintResizeToSdSize;
		wxStaticText* m_staticText69;
		wxButton* m_generate1;
		wxFilePickerCtrl* m_img2imgOpen;
		wxStaticText* m_staticText24;
		wxSpinCtrlDouble* m_strength;
		wxButton* m_img2im_preview_img;
		wxBitmapButton* m_delete_initial_img;
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
		wxPanel* m_imageinfo;
		wxScrolledWindow* m_scrolledWindow5;
		wxStaticBitmap* m_imageinfo_preview;
		wxScrolledWindow* m_scrolledWindow7;
		wxTextCtrl* m_imageInfoPrompt;
		wxTextCtrl* m_imageInfoNegPrompt;
		wxTextCtrl* m_imageInfoList;
		wxButton* m_imageInfoLoadTotxt;
		wxButton* m_imageInfoLoadToimg2img;
		wxFilePickerCtrl* m_imageInfoOpen;
		wxBitmapButton* m_cleanImageInfo;
		wxPanel* m_models_panel;
		wxCheckBox* m_checkbox_lora_filter;
		wxCheckBox* m_checkbox_filter_checkpoints;
		wxCheckBox* m_checkbox_filter_embeddings;
		wxSearchCtrl* m_modellist_filter;
		wxSplitterWindow* m_splitter3;
		wxPanel* m_panel16;
		wxDataViewListCtrl* m_data_model_list;
		wxDataViewColumn* m_dataViewListColumn3;
		wxDataViewColumn* m_dataViewListColumn4;
		wxDataViewColumn* m_dataViewListColumn5;
		wxDataViewColumn* m_dataViewListColumn6;
		wxDataViewColumn* m_dataViewListColumn8;
		wxDataViewColumn* m_dataViewListColumn7;
		wxPanel* m_panel17;
		wxDataViewListCtrl* m_model_details;
		wxDataViewColumn* m_dataViewListColumn11;
		wxDataViewColumn* m_dataViewListColumn21;
		wxHtmlWindow* m_model_details_description;
		wxScrolledWindow* m_scrolledWindow4;
		wxTextCtrl* logs;
		wxStatusBar* m_statusBar166;

		// Virtual event handlers, override them in your derived class
		virtual void onSettings( wxCommandEvent& event ) { event.Skip(); }
		virtual void onModelsRefresh( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAboutButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCivitAitButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnShowWidget( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStopBackgroundProcess( wxCommandEvent& event ) { event.Skip(); }
		virtual void onModelSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onTypeSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVaeSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVAEOnCpu( wxCommandEvent& event ) { event.Skip(); }
		virtual void onClipOnCpu( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSamplerSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onRandomGenerateButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnWHChange( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnWHChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void onResolutionSwap( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSavePreset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onLoadPreset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSelectPreset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDeletePreset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSd15ResSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSdXLResSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_notebook1302OnNotebookPageChanged( wxNotebookEvent& event ) { event.Skip(); }
		virtual void onJobsStart( wxCommandEvent& event ) { event.Skip(); }
		virtual void onJobsPause( wxCommandEvent& event ) { event.Skip(); }
		virtual void onJobsDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnJobListItemActivated( wxDataViewEvent& event ) { event.Skip(); }
		virtual void onContextMenu( wxDataViewEvent& event ) { event.Skip(); }
		virtual void OnJobListItemSelection( wxDataViewEvent& event ) { event.Skip(); }
		virtual void onTxt2ImgFileDrop( wxDropFilesEvent& event ) { event.Skip(); }
		virtual void OnPromptText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNegPromptText( wxCommandEvent& event ) { event.Skip(); }
		virtual void onWhatIsThis( wxCommandEvent& event ) { event.Skip(); }
		virtual void onFilePickerDiffusionModel( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void onDiffusionFlashAttn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCleanDiffusionModel( wxCommandEvent& event ) { event.Skip(); }
		virtual void onFilePickerClipL( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void onFilePickerClipG( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void onFilePickerT5XXL( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void onGenerate( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnControlnetImageOpen( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void onCnOnCpu( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnControlnetImagePreviewButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnControlnetImageDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void Onimg2imgDropFile( wxDropFilesEvent& event ) { event.Skip(); }
		virtual void OnInpaintSaveMask( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnInpaintResizeImage( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnImg2ImgMouseEnter( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnImg2ImgMouseLeave( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnImg2ImgMouseDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnImg2ImgMouseUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnImg2ImgMouseMotion( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnImg2ImgMouseWheel( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnImg2ImgPaint( wxPaintEvent& event ) { event.Skip(); }
		virtual void OnImageOpenFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnImg2ImgPreviewButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteInitialImage( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpscalerDropFile( wxDropFilesEvent& event ) { event.Skip(); }
		virtual void OnImageOpenFilePickerChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnDeleteUpscaleImage( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpscalerModelSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpscalerFactorChange( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnHtmlLinkClicked( wxHtmlLinkEvent& event ) { event.Skip(); }
		virtual void OnImageInfoDrop( wxDropFilesEvent& event ) { event.Skip(); }
		virtual void OnImageInfoLoadTxt2img( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnImageInfoLoadImg2img( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnImageInfoOpen( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnCleanImageInfo( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckboxLoraFilter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckboxCheckpointFilter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnModellistFilterKeyUp( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnDataModelActivated( wxDataViewEvent& event ) { event.Skip(); }
		virtual void OnDataModelSelected( wxDataViewEvent& event ) { event.Skip(); }


	public:
		wxBoxSizer* bSizer8911;
		wxScrolledWindow* m_img2imPanel;
		wxHtmlWindow* m_upscalerHelp;
		wxSplitterWindow* m_splitter4;
		wxBoxSizer* imageInfoSizer;
		wxBoxSizer* bSizer119;
		wxFlexGridSizer* fgSizer1;
		wxBoxSizer* bSizer1001;
		wxBoxSizer* bSizer891;

		mainUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("sd.cpp.gui"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1024,768 ), long style = wxDEFAULT_FRAME_STYLE|wxBORDER_DEFAULT, const wxString& name = wxT("sd.cpp.gui") );

		~mainUI();

		void m_splitter2OnIdle( wxIdleEvent& )
		{
			m_splitter2->SetSashPosition( -200 );
			m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( mainUI::m_splitter2OnIdle ), NULL, this );
		}

		void m_splitter4OnIdle( wxIdleEvent& )
		{
			m_splitter4->SetSashPosition( 0 );
			m_splitter4->Disconnect( wxEVT_IDLE, wxIdleEventHandler( mainUI::m_splitter4OnIdle ), NULL, this );
		}

		void m_splitter3OnIdle( wxIdleEvent& )
		{
			m_splitter3->SetSashPosition( -300 );
			m_splitter3->Disconnect( wxEVT_IDLE, wxIdleEventHandler( mainUI::m_splitter3OnIdle ), NULL, this );
		}

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
		wxBitmapButton* m_openModelsPath;
		wxStaticText* m_staticText18013172027;
		wxDirPickerCtrl* m_lora_dir;
		wxBitmapButton* m_openLorasPath;
		wxStaticText* m_staticText18013;
		wxDirPickerCtrl* m_vae_dir;
		wxBitmapButton* m_openVaesPath;
		wxStaticText* m_staticText1801317202731;
		wxDirPickerCtrl* m_embedding_dir;
		wxBitmapButton* m_openEmbeddingsPath;
		wxStaticText* m_staticText21;
		wxDirPickerCtrl* m_taesd_dir;
		wxBitmapButton* m_openTaesdPath;
		wxBitmapButton* m_bpButton1;
		wxStaticText* m_staticText23;
		wxDirPickerCtrl* m_controlnet_dir;
		wxBitmapButton* m_openControlnetPath;
		wxStaticText* m_staticText77;
		wxDirPickerCtrl* m_esrgan_dir;
		wxBitmapButton* m_openEsrganPath;
		wxStaticLine* m_staticLine223;
		wxStaticText* m_staticText19;
		wxDirPickerCtrl* m_presets_dir;
		wxBitmapButton* m_openPresetsPath;
		wxStaticText* m_staticText180131720;
		wxDirPickerCtrl* m_images_output;
		wxBitmapButton* m_openOutputPath;
		wxPanel* m_settings;
		wxCheckBox* m_autogen_hash;
		wxCheckBox* m_keep_model_in_memory;
		wxCheckBox* m_save_all_image;
		wxStaticText* m_staticText16;
		wxChoice* m_image_type;
		wxStaticText* m_staticText38;
		wxStaticText* m_staticText61;
		wxSlider* m_image_quality;
		wxStaticText* m_staticText62;
		wxSlider* m_png_compression;
		wxStaticText* m_staticText191;
		wxSpinCtrl* m_threads;
		wxStaticText* m_staticText621;
		wxBitmapButton* m_bpButton27;
		wxTextCtrl* m_output_filename_format;
		wxPanel* m_settings_ui;
		wxCheckBox* m_show_notifications;
		wxStaticText* m_staticText60;
		wxSpinCtrl* m_notification_timeout;
		wxStaticText* m_staticText50;
		wxChoice* m_language;
		wxPanel* m_panel18;
		wxCheckBox* m_enableCivitai;
		wxStaticText* m_staticText42;
		wxTextCtrl* m_civitai_api_key;
		wxBitmapButton* m_bpButton15;
		wxButton* m_save;

		// Virtual event handlers, override them in your derived class
		virtual void OnOpenFolder( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTAESDHelpClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnImgQualityScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnPngCompressionScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnOutputFileNameFormatHelpClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOutputFilenameText( wxCommandEvent& event ) { event.Skip(); }
		virtual void onShowNotificationCheck( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCivitaiHelpButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSave( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxStaticText* m_staticNumberOfCores;

		Settings( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxFRAME_FLOAT_ON_PARENT|wxRESIZE_BORDER|wxSTAY_ON_TOP|wxSYSTEM_MENU|wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL|wxBORDER_THEME, const wxString& name = wxT("sd.cpp.gui.settings") );

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

///////////////////////////////////////////////////////////////////////////////
/// Class AboutDialog
///////////////////////////////////////////////////////////////////////////////
class AboutDialog : public wxDialog
{
	private:

	protected:

		// Virtual event handlers, override them in your derived class
		virtual void m_aboutOnHtmlLinkClicked( wxHtmlLinkEvent& event ) { event.Skip(); }


	public:
		wxHtmlWindow* m_about;

		AboutDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("About"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,600 ), long style = wxDEFAULT_DIALOG_STYLE );

		~AboutDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class CivitAiWindow
///////////////////////////////////////////////////////////////////////////////
class CivitAiWindow : public wxFrame
{
	private:

	protected:
		wxPanel* m_panel16;
		wxSplitterWindow* m_splitter3;
		wxPanel* m_panel25;
		wxHtmlWindow* m_model_description;
		wxStaticText* m_staticText41;
		wxDataViewListCtrl* m_model_details;
		wxDataViewColumn* m_dataViewListColumn111;
		wxDataViewColumn* m_dataViewListColumn22;
		wxDataViewColumn* m_dataViewListColumn21;
		wxDataViewColumn* m_dataViewListColumn33;
		wxDataViewColumn* m_dataViewListColumn34;
		wxDataViewColumn* m_dataViewListColumn221;
		wxHtmlWindow* m_model_version_description;
		wxStaticText* m_staticText40;
		wxButton* m_model_download;
		wxDataViewListCtrl* m_model_filelist;
		wxDataViewColumn* m_dataViewListColumn1111;
		wxDataViewColumn* m_dataViewListColumn211;
		wxDataViewColumn* m_dataViewListColumn23;
		wxDataViewColumn* m_dataViewListColumn24;
		wxDataViewColumn* m_dataViewListColumn25;
		wxDataViewColumn* m_dataViewListColumn231;
		wxDataViewColumn* m_dataViewListColumn29;
		wxPanel* m_panel24;
		wxRadioBox* m_model_type;
		wxTextCtrl* m_civitai_search;
		wxButton* m_search;
		wxStaticText* m_staticText43;
		wxDataViewListCtrl* m_dataViewListCtrl5;
		wxDataViewColumn* m_dataViewListColumn12;
		wxDataViewColumn* m_dataViewListColumn13;
		wxDataViewColumn* m_dataViewListColumn30;
		wxDataViewColumn* m_dataViewListColumn32;
		wxStaticText* m_staticText431;
		wxDataViewListCtrl* m_downloads;
		wxDataViewColumn* m_dataViewListColumn26;
		wxDataViewColumn* m_dataViewListColumn31;
		wxDataViewColumn* m_dataViewListColumn301;
		wxDataViewColumn* m_dataViewListColumn28;
		wxStatusBar* m_statusBar2;

		// Virtual event handlers, override them in your derived class
		virtual void OnHtmlLinkClicked( wxHtmlLinkEvent& event ) { event.Skip(); }
		virtual void m_model_detailsOnDataViewListCtrlSelectionChanged( wxDataViewEvent& event ) { event.Skip(); }
		virtual void m_model_downloadOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_model_filelistOnDataViewListCtrlSelectionChanged( wxDataViewEvent& event ) { event.Skip(); }
		virtual void m_civitai_searchOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_searchOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_dataViewListCtrl5OnDataViewListCtrlSelectionChanged( wxDataViewEvent& event ) { event.Skip(); }


	public:
		wxScrolledWindow* m_scrolledWindow4;
		wxBoxSizer* image_container;

		CivitAiWindow( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("CivitAI model downloader"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~CivitAiWindow();

		void m_splitter3OnIdle( wxIdleEvent& )
		{
			m_splitter3->SetSashPosition( 0 );
			m_splitter3->Disconnect( wxEVT_IDLE, wxIdleEventHandler( CivitAiWindow::m_splitter3OnIdle ), NULL, this );
		}

};

///////////////////////////////////////////////////////////////////////////////
/// Class DesktopWidget
///////////////////////////////////////////////////////////////////////////////
class DesktopWidget : public wxFrame
{
	private:

	protected:
		wxStaticText* m_currentStatus;
		wxGauge* m_currentProgress;

		// Virtual event handlers, override them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnMouseEnter( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnIconize( wxIconizeEvent& event ) { event.Skip(); }
		virtual void OnMouseLeave( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnLeftMouseDClick( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseLeftUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseMotion( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnWidgetPaint( wxPaintEvent& event ) { event.Skip(); }


	public:

		DesktopWidget( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,68 ), long style = wxFRAME_NO_TASKBAR|wxFRAME_SHAPED|wxSTAY_ON_TOP|wxBORDER_NONE|wxTRANSPARENT_WINDOW, const wxString& name = wxT("DesktopWidget") );

		~DesktopWidget();

};

