#ifndef __MainWindowUI__
#define __MainWindowUI__

/**
@file
Subclass of UI, which is generated by wxFormBuilder.
*/

#include "MainWindow.h"

//// end generated include
#include "ver.hpp"
#include "../cores.h"
#include "MainWindowSettings.h"
#include "MainWindowImageViewer.h"
#include "QueueManager.h"

#include <filesystem>
#include <map>
#include <thread>
#include <vector>
#include <fstream>
#include <mutex>

#include <fmt/format.h>
#include "utils.hpp"
#include <stable-diffusion.h>

#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <wx/event.h>
#include <wx/notifmsg.h>
#include <wx/textdlg.h>
#include <wx/menu.h>

/** Implementing UI */
class MainWindowUI : public UI
{
	protected:
		// Handlers for UI events.
		void onSettings( wxCommandEvent& event );
		void onModelsRefresh( wxCommandEvent& event );
		void onModelSelect( wxCommandEvent& event );
		void onVaeSelect( wxCommandEvent& event );
		void onRandomGenerateButton( wxCommandEvent& event );
		void onResolutionSwap( wxCommandEvent& event );
		void onJobsStart( wxCommandEvent& event );
		void onJobsPause( wxCommandEvent& event );
		void onJobsDelete( wxCommandEvent& event );
		void onJoblistItemActivated( wxDataViewEvent& event );
		void onContextMenu( wxDataViewEvent& event );
		void onJoblistSelectionChanged( wxDataViewEvent& event );
		void onGenerate( wxCommandEvent& event );
		void onSamplerSelect( wxCommandEvent& event );
		void onSavePreset( wxCommandEvent& event );
		void onLoadPreset( wxCommandEvent& event );
		void onSelectPreset( wxCommandEvent& event );
		void onDeletePreset( wxCommandEvent& event );
	public:
		/** Constructor */
		MainWindowUI( wxWindow* parent );
	//// end generated class members
	~MainWindowUI();
	void OnThreadMessage(wxThreadEvent &e);

private:
	std::mutex sdMutex;
	wxFileConfig *fileConfig;
	sd_gui_utils::config *cfg;
	wxString ini_path;
	MainWindowSettings *settingsWindow;
	sd_gui_utils::SDParams *sd_params;

	std::map<std::string, std::string> ModelFiles;
	std::map<std::string, std::string> VaeFiles;
	std::map<std::string, sd_gui_utils::generator_preset> Presets;

	// the queue manager
	QM::QueueManager *qmanager;
	bool modelLoaded = false;
	bool firstCfgInit = true;
	std::string currentModel;
	std::string currentVaeModel;
	sd_ctx_t *sd_ctx;
	std::streambuf *buffer;
	std::vector<std::thread*> threads;

	// row,QueueItem
	std::map<int, QM::QueueItem*> JobTableItems;
	std::map<int, wxDataViewColumn *> *JobTableColumns;

	void initConfig();
	void loadModelList();
	void loadVaeList();
	void OnCloseSettings(wxCloseEvent &event);
	void OnPopupClick(wxCommandEvent &evt);
	void LoadFileList(sd_gui_utils::DirTypes type = sd_gui_utils::DirTypes::CHECKPOINT);
	void LoadPresets();

	static void HandleSDLog(sd_log_level_t level, const char *text, void *data);
	static void HandleSDProgress(int step, int steps, float time, void *data);

	// load the model in a new thread
	sd_ctx_t *LoadModelv2(wxEvtHandler *eventHandler, QM::QueueItem myItem);
	// generate in another thread
	void Generate(wxEvtHandler *eventHandler, QM::QueueItem myItem);

	// start a thread to generate image
	void StartGeneration(QM::QueueItem myJob);

	// handle queue managers events, manipulate data table by events
	void OnQueueItemManagerItemAdded(QM::QueueItem item);
	void OnQueueItemManagerItemUpdated(QM::QueueItem item);
	void OnQueueItemManagerItemStatusChanged(QM::QueueItem item);

	wxNotificationMessage *notification;



};

#endif // __MainWindowUI__
