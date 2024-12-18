#ifndef __MainWindowSettings__
#define __MainWindowSettings__

/**
@file
Subclass of Settings, which is generated by wxFormBuilder.
*/

/** Implementing Settings */

class MainWindowSettings : public Settings {
protected:
    // Handlers for Settings events.
    void OnImgQualityScroll(wxScrollEvent& event) override;
    void OnPngCompressionScroll(wxScrollEvent& event) override;
    void onShowNotificationCheck(wxCommandEvent& event) override;
    void OnCivitaiHelpButton(wxCommandEvent& event) override;
    void onSave(wxCommandEvent& event) override;
    void OnTAESDHelpClick(wxCommandEvent& event) override;
    void OnOutputFileNameFormatHelpClick(wxCommandEvent& event) override;
    void OnOpenFolder(wxCommandEvent& event) override;
    void OnAddServer(wxCommandEvent& event) override;
    void OnDeleteServer(wxCommandEvent& event) override;
    void OnServerListStartEditing(wxDataViewEvent& event) override;
    void OnServerListEditingStarted(wxDataViewEvent& event) override;
    void OnServerListEditingDone(wxDataViewEvent& event) override;
    void OnServerListSelectionChanged(wxDataViewEvent& event) override;
    void OnServerListItemValueChanged(wxDataViewEvent& event) override;

public:
    /** Constructor */
    MainWindowSettings(wxWindow* parent, wxConfigBase* config, sd_gui_utils::config* cfg);
    ~MainWindowSettings();
    //// end generated class members
private:
    void InitConfig();
    void AddRemoteServerToList(sd_gui_utils::sdServer& server);
    wxConfigBase* config;
    sd_gui_utils::config* cfg;
    std::string ini_path;
    std::map<int, std::string> locales;
    enum ServerListColumns {
        SERVER_LIST_COLUMN_ENABLE = 0,
        SERVER_LIST_COLUMN_HOST,
        SERVER_LIST_COLUMN_PORT,
        SERVER_LIST_COLUMN_AUTH_KEY,
        SERVER_LIST_COLUMN_STATUS
    };
};

#endif  // __MainWindowSettings__
