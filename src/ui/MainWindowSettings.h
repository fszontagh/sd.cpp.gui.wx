#ifndef __MainWindowSettings__
#define __MainWindowSettings__

/**
@file
Subclass of Settings, which is generated by wxFormBuilder.
*/

#include "MainWindow.h"

//// end generated include
#include "../libs/cores.h"
#include "utils.hpp"
#include "ver.hpp"

#include <wx/fileconf.h>
#include <wx/secretstore.h>
#include <wx/stdpaths.h>

/** Implementing Settings */
class MainWindowSettings : public Settings {
protected:
    // Handlers for Settings events.
    void OnImgQualityScroll(wxScrollEvent& event) override;
    void OnImgQualitySpin(wxSpinEvent& event) override;
    void onShowNotificationCheck(wxCommandEvent& event) override;
    void OnCivitaiHelpButton(wxCommandEvent& event) override;
    void onSave(wxCommandEvent& event) override;
	void OnTAESDHelpClick(wxCommandEvent& event) override;
	void OnOpenFolder(wxCommandEvent& event) override;

public:
    /** Constructor */
    MainWindowSettings(wxWindow* parent);
    //// end generated class members
private:
    void InitConfig();
    wxFileConfig* fileConfig;
    sd_gui_utils::config* cfg;
    std::string ini_path;
};

#endif  // __MainWindowSettings__
