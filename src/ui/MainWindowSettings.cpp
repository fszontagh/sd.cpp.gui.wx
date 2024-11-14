#include "MainWindowSettings.h"
#include "wx/translation.h"
#include "wx/uilocale.h"
#include "wx/utils.h"

MainWindowSettings::MainWindowSettings(wxWindow* parent)
    : Settings(parent) {
    this->ini_path = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + "sd.ui.config.ini";

    this->cfg        = new sd_gui_utils::config;
    this->fileConfig = new wxFileConfig("sd.cpp.ui", wxEmptyString, this->ini_path);
    wxConfigBase::Set(fileConfig);
    this->InitConfig();
}

void MainWindowSettings::OnImgQualityScroll(wxScrollEvent& event) {
    // TODO: Implement OnImgQualityScroll
    this->m_image_quality_spin->SetValue(this->m_image_quality->GetValue());
}

void MainWindowSettings::OnImgQualitySpin(wxSpinEvent& event) {
    // TODO: Implement OnImgQualitySpin
    this->m_image_quality->SetValue(this->m_image_quality_spin->GetValue());
}

void MainWindowSettings::onShowNotificationCheck(wxCommandEvent& event) {
    // TODO: Implement onShowNotificationCheck
    if (!this->m_show_notifications->IsChecked()) {
        this->m_notification_timeout->Disable();
        this->cfg->notification_timeout = 60;
    } else {
        this->m_notification_timeout->Enable();
    }
}

void MainWindowSettings::OnCivitaiHelpButton(wxCommandEvent& event) {
    wxString civitHelpLink = "https://education.civitai.com/civitais-guide-to-downloading-via-api/#step-by-step";
    wxLaunchDefaultBrowser(civitHelpLink);
}

void MainWindowSettings::OnTAESDHelpClick(wxCommandEvent& event) {
    wxString taesdHelpLink = "https://github.com/leejet/stable-diffusion.cpp/blob/master/docs/taesd.md";
    wxLaunchDefaultBrowser(taesdHelpLink);
}

void MainWindowSettings::OnOpenFolder(wxCommandEvent& event) {
    if (event.GetEventObject() == this->m_openModelsPath) {
        wxLaunchDefaultApplication(this->m_model_dir->GetPath());
    }
    if (event.GetEventObject() == this->m_openLorasPath) {
        wxLaunchDefaultApplication(this->m_lora_dir->GetPath());
    }
    if (event.GetEventObject() == this->m_openVaesPath) {
        wxLaunchDefaultApplication(this->m_vae_dir->GetPath());
    }
    if (event.GetEventObject() == this->m_openEmbeddingsPath) {
        wxLaunchDefaultApplication(this->m_embedding_dir->GetPath());
    }
    if (event.GetEventObject() == this->m_openTaesdPath) {
        wxLaunchDefaultApplication(this->m_taesd_dir->GetPath());
    }
    if (event.GetEventObject() == this->m_openEsrganPath) {
        wxLaunchDefaultApplication(this->m_esrgan_dir->GetPath());
    }
    if (event.GetEventObject() == this->m_openControlnetPath) {
        wxLaunchDefaultApplication(this->m_controlnet_dir->GetPath());
    }
    if (event.GetEventObject() == this->m_openPresetsPath) {
        wxLaunchDefaultApplication(this->m_presets_dir->GetPath());
    }
    if (event.GetEventObject() == this->m_openOutputPath) {
        wxLaunchDefaultApplication(this->m_images_output->GetPath());
    }
}

void MainWindowSettings::onSave(wxCommandEvent& event) {
    // save secret
    wxString username = "civitai_api_key";

    if (!this->m_civitai_api_key->GetValue().empty()) {
        wxSecretValue password(this->m_civitai_api_key->GetValue());
        wxSecretStore store = wxSecretStore::GetDefault();
        store.Save(SD_GUI_HOMEPAGE, username, password);
    }

    this->fileConfig->Write("/paths/lora", this->m_lora_dir->GetPath());
    this->fileConfig->Write("/paths/model", this->m_model_dir->GetPath());
    this->fileConfig->Write("/paths/vae", this->m_vae_dir->GetPath());
    this->fileConfig->Write("/paths/embedding", this->m_embedding_dir->GetPath());
    this->fileConfig->Write("/paths/taesd", this->m_taesd_dir->GetPath());
    this->fileConfig->Write("/paths/esrgan", this->m_esrgan_dir->GetPath());
    this->fileConfig->Write("/paths/controlnet", this->m_controlnet_dir->GetPath());
    this->fileConfig->Write("/paths/presets", this->m_presets_dir->GetPath());
    this->fileConfig->Write("/keep_model_in_memory", this->m_keep_model_in_memory->GetValue());
    this->fileConfig->Write("/save_all_image", this->m_save_all_image->GetValue());
    this->fileConfig->Write("/n_threads", this->m_threads->GetValue());
    this->fileConfig->Write("/paths/output", this->m_images_output->GetPath());
    this->fileConfig->Write("/image_quality", this->m_image_quality->GetValue());
    this->fileConfig->Write("/image_type", sd_gui_utils::image_types_str[this->m_image_type->GetSelection()]);
    this->fileConfig->Write("/show_notification", this->m_show_notifications->GetValue());
    this->fileConfig->Write("/notification_timeout", this->m_notification_timeout->GetValue());
    this->fileConfig->Write("/enable_civitai", this->m_enableCivitai->GetValue());
    
    auto language = this->locales[this->m_language->GetSelection()];
    this->fileConfig->Write("/language", wxString::FromUTF8Unchecked(language));
    this->fileConfig->Flush();
    this->Close();
}

void MainWindowSettings::InitConfig() {
    this->m_staticNumberOfCores->SetLabel(wxString::Format("%d", cores()));
    wxString datapath   = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "sd_ui_data" + wxFileName::GetPathSeparator();
    wxString imagespath = wxStandardPaths::Get().GetDocumentsDir() + wxFileName::GetPathSeparator() + "sd_ui_output" + wxFileName::GetPathSeparator();

    wxString model_path = datapath;
    model_path.append("checkpoints");

    wxString vae_path = datapath;
    vae_path.append("vae");

    wxString lora_path = datapath;
    lora_path.append("lora");

    wxString embedding_path = datapath;
    embedding_path.append("embedding");

    wxString taesd_path = datapath;
    taesd_path.append("taesd");

    wxString presets_path = datapath;
    presets_path.append("presets");

    wxString controlnet_path = datapath;
    controlnet_path.append("controlnet");

    wxString esrgan_path = datapath;
    esrgan_path.append("esrgan");

    this->cfg->lora                 = this->fileConfig->Read("/paths/lora", lora_path).ToStdString();
    this->cfg->model                = this->fileConfig->Read("/paths/model", model_path).ToStdString();
    this->cfg->vae                  = this->fileConfig->Read("/paths/vae", vae_path).ToStdString();
    this->cfg->embedding            = this->fileConfig->Read("/paths/embedding", embedding_path).ToStdString();
    this->cfg->taesd                = this->fileConfig->Read("/paths/taesd", taesd_path).ToStdString();
    this->cfg->esrgan               = this->fileConfig->Read("/paths/esrgan", esrgan_path).ToStdString();
    this->cfg->controlnet           = this->fileConfig->Read("/paths/controlnet", controlnet_path).ToStdString();
    this->cfg->presets              = this->fileConfig->Read("/paths/presets", presets_path).ToStdString();
    this->cfg->output               = this->fileConfig->Read("/paths/output", imagespath).ToStdString();
    this->cfg->keep_model_in_memory = this->fileConfig->Read("/keep_model_in_memory", this->cfg->keep_model_in_memory);
    this->cfg->save_all_image       = this->fileConfig->Read("/save_all_image", this->cfg->save_all_image);
    this->cfg->n_threads            = this->fileConfig->Read("/n_threads", cores());
    this->cfg->image_quality        = this->fileConfig->Read("/image_quality", this->cfg->image_quality);
    this->cfg->show_notifications   = this->fileConfig->ReadBool("/show_notification", this->cfg->show_notifications);
    this->cfg->notification_timeout = this->fileConfig->Read("/notification_timeout", this->cfg->notification_timeout);
    this->cfg->enable_civitai       = this->fileConfig->ReadBool("/enable_civitai", this->cfg->enable_civitai);
    this->cfg->language             = this->fileConfig->Read("/language", wxUILocale::GetLanguageInfo(wxUILocale::GetSystemLocale())->CanonicalName.utf8_string());
    std::cout << "Lang: " << this->cfg->language << std::endl;

    wxString username = "civitai_api_key";
    wxSecretValue password;

    wxSecretStore store = wxSecretStore::GetDefault();

    if (store.Load(SD_GUI_HOMEPAGE, username, password)) {
        this->m_civitai_api_key->SetValue(password.GetAsString());
    }

    int idx               = 0;
    auto saved_image_type = this->fileConfig->Read("/image_type", "JPG");

    for (auto type : sd_gui_utils::image_types_str) {
        if (saved_image_type == type) {
            this->m_image_type->Select(idx);
            this->cfg->image_type = (sd_gui_utils::imageTypes)idx;
            break;
        }
        idx++;
    }

    this->m_lora_dir->SetPath(this->cfg->lora);
    this->m_model_dir->SetPath(this->cfg->model);
    this->m_vae_dir->SetPath(this->cfg->vae);
    this->m_embedding_dir->SetPath(this->cfg->embedding);
    this->m_taesd_dir->SetPath(this->cfg->taesd);
    this->m_esrgan_dir->SetPath(this->cfg->esrgan);
    this->m_controlnet_dir->SetPath(this->cfg->controlnet);
    this->m_presets_dir->SetPath(this->cfg->presets);
    this->m_images_output->SetPath(this->cfg->output);
    this->m_keep_model_in_memory->SetValue(this->cfg->keep_model_in_memory);
    this->m_save_all_image->SetValue(this->cfg->save_all_image);
    this->m_threads->SetValue(this->cfg->n_threads);
    this->m_image_quality->SetValue(this->cfg->image_quality);
    this->m_image_quality_spin->SetValue(this->cfg->image_quality);
    this->m_image_type->Select((int)this->cfg->image_type);
    this->m_show_notifications->SetValue(this->cfg->show_notifications);
    this->m_notification_timeout->SetValue(this->cfg->notification_timeout);
    this->m_enableCivitai->SetValue(this->cfg->enable_civitai);

    //  populate available languages
    /*this->m_language->Append("English");
    this->m_language->Append("简体中文");
    this->m_language->Append("繁體中文");
    this->m_language->Append("日本語");
    this->m_language->Append("Español");
    this->m_language->Append("Français");
    this->m_language->Append("Deutsch");
    this->m_language->Append("Italiano");
    this->m_language->Append("Português");
    this->m_language->Append("Русский");
    this->m_language->Append("日本語");
    this->m_language->Append("한국어");
    this->m_language->Append("中文");
    this->m_language->Append("日本語");
    this->m_language->Append("中文");*/

    auto tr     = wxTranslations::Get();
    auto locale = wxUILocale::GetCurrent();

    auto systemLocale = wxUILocale::GetLanguageInfo(wxUILocale::GetSystemLocale())->CanonicalName;

    auto langs = tr->GetAvailableTranslations("stablediffusiongui");

    int selected = 0;
    int counter  = 0;
    std::cout << " system locale: " << systemLocale.utf8_string() << std::endl;

    for (const auto lang : langs) {
        auto langInfo = wxUILocale::FindLanguageInfo(lang);
        wxString info = "";
        if (langInfo != nullptr) {
            info.Append(langInfo->DescriptionNative);
            

            std::cout << "LOCALE: " << langInfo->CanonicalRef.utf8_string() << " == " << systemLocale.utf8_string() << std::endl;

            if (langInfo->CanonicalName.utf8_string() == this->cfg->language) {
                selected = counter;
            }
            this->locales[counter] = langInfo->CanonicalName.utf8_string();
            this->m_language->Append(info);
            counter++;
        }
    }
    this->m_language->SetSelection(selected);
}