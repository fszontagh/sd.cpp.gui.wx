#ifndef __MainWindowCivitAiWindow__
#define __MainWindowCivitAiWindow__

/**
@file
Subclass of CivitAiWindow, which is generated by wxFormBuilder.
*/

#include "MainWindow.h"

//// end generated include
#include "ver.hpp"

#include <wx/msgdlg.h>
#include <wx/secretstore.h>

#include <sstream>
#include <thread>
#include <vector>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include "../libs/json.hpp"
#include "../helpers/civitaiHttp.hpp"
#include "utils.hpp"
#include "ModelInfo.h"

/** Implementing CivitAiWindow */
class MainWindowCivitAiWindow : public CivitAiWindow
{
protected:
	// Handlers for CivitAiWindow events.
	void m_civitai_searchOnTextEnter(wxCommandEvent &event);
	void m_searchOnButtonClick(wxCommandEvent &event);
	void m_dataViewListCtrl5OnDataViewListCtrlSelectionChanged(wxDataViewEvent &event);
	void OnHtmlLinkClicked(wxHtmlLinkEvent &event);
	void m_model_detailsOnDataViewListCtrlSelectionChanged(wxDataViewEvent &event);
	void m_model_downloadOnButtonClick(wxCommandEvent &event);
	void m_model_filelistOnDataViewListCtrlSelectionChanged(wxDataViewEvent &event);

public:
	/** Constructor */
	MainWindowCivitAiWindow(wxWindow *parent);
	//// end generated class members

	~MainWindowCivitAiWindow();
	void SetModelManager(ModelInfo::Manager *manager);
	void SetCfg(sd_gui_utils::config *cfg);

private:
	bool isLoading = false;
	std::string current_json_text;
	sd_gui_utils::config *config;
	std::vector<std::thread *> modelDownloadThreads;
	std::vector<std::thread *> infoDownloadThread;
	std::vector<std::thread *> imgDownloadThreads;
	std::vector<nlohmann::json *> loadedJsonDatas;
	nlohmann::json currentModelVersions;
	std::vector<nlohmann::json *> modelDetailData;
	ModelInfo::Manager *ModelInfoManager;
	std::vector<sd_gui_utils::ModelFileInfo *> localAvailableFiles;
	std::vector<CivitAi::DownloadItem *> downloads;
	std::map<int, CivitAi::PreviewImage *> previewImages;
	std::vector<wxStaticBitmap *> bitmaps;
	int current_version_id = 0;

	void civitSearchThread(std::string query);
	void imgDownloadThread(CivitAi::PreviewImage *previewImage, int versionId);
	void modelDownloadThread(CivitAi::DownloadItem *item);
	void OnSearch(std::string query);
	void OnThreadMessage(wxThreadEvent &e);
	void JsonToTable(std::string json_str);
	bool CheckIfModelDownloaded(nlohmann::json item);

	void populateVersions(nlohmann::json js);
	void populateFiles(nlohmann::json js);

	void loadImages(nlohmann::json js);
	void showImages(int version_id, bool from_thread = false);

	void SendThreadEvent(std::string payload);
	void SendThreadEvent(wxString payload);
	void SendThreadEvent(std::string str, int id, CivitAi::PreviewImage *payload);

	template <typename T>
	inline void SendThreadEvent(std::string str, T *payload)
	{
		wxThreadEvent *event = new wxThreadEvent();
		event->SetPayload(payload);
		event->SetString(str);
		wxQueueEvent(this->GetEventHandler(), event);
	}

	inline static size_t curlCppWriteDataToFile(char *ptr, size_t size, size_t nmemb, void *f)
	{
		CivitAi::DownloadItem *item = (CivitAi::DownloadItem *)f;
		size_t written = size * nmemb;
		item->file->write(ptr, written);
		item->downloadedSize += written;
		return written;
	}

	void OnTimer(wxTimerEvent &e);
};

#endif // __MainWindowCivitAiWindow__
