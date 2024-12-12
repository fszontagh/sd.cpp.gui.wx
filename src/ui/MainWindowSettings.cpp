#include "MainWindowSettings.h"

MainWindowSettings::MainWindowSettings(wxWindow* parent, wxConfigBase* config, sd_gui_utils::config* cfg)
    : Settings(parent), config(config), cfg(cfg) {
    this->InitConfig();
}
MainWindowSettings::~MainWindowSettings() {
}

void MainWindowSettings::OnImgQualityScroll(wxScrollEvent& event) {
    this->cfg->image_quality = this->m_image_quality->GetValue();
}

void MainWindowSettings::OnPngCompressionScroll(wxScrollEvent& event) {
    this->cfg->png_compression_level = this->m_png_compression->GetValue();
}
void MainWindowSettings::onShowNotificationCheck(wxCommandEvent& event) {
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
void MainWindowSettings::OnOutputFileNameFormatHelpClick(wxCommandEvent& event) {
    wxString helpLink = "https://github.com/fszontagh/sd.cpp.gui.wx/wiki/GUI-howtos#image-output-filename-format";
    wxLaunchDefaultBrowser(helpLink);
}

void MainWindowSettings::OnOpenFolder(wxCommandEvent& event) {
    auto object = reinterpret_cast<wxBitmapButton*>(event.GetEventObject());

    if (object == this->m_openModelsPath) {
        wxLaunchDefaultApplication(this->m_model_dir->GetPath());
    }
    if (object == this->m_openLorasPath) {
        wxLaunchDefaultApplication(this->m_lora_dir->GetPath());
    }
    if (object == this->m_openVaesPath) {
        wxLaunchDefaultApplication(this->m_vae_dir->GetPath());
    }
    if (object == this->m_openEmbeddingsPath) {
        wxLaunchDefaultApplication(this->m_embedding_dir->GetPath());
    }
    if (object == this->m_openTaesdPath) {
        wxLaunchDefaultApplication(this->m_taesd_dir->GetPath());
    }
    if (object == this->m_openEsrganPath) {
        wxLaunchDefaultApplication(this->m_esrgan_dir->GetPath());
    }
    if (object == this->m_openControlnetPath) {
        wxLaunchDefaultApplication(this->m_controlnet_dir->GetPath());
    }
    if (object == this->m_openPresetsPath) {
        wxLaunchDefaultApplication(this->m_presets_dir->GetPath());
    }
    if (object == this->m_openOutputPath) {
        wxLaunchDefaultApplication(this->m_images_output->GetPath());
    }
}

void MainWindowSettings::onSave(wxCommandEvent& event) {
    // save secret
    wxString username = "civitai_api_key";

    if (!this->m_civitai_api_key->GetValue().empty()) {
        wxSecretValue password(this->m_civitai_api_key->GetValue());
        wxSecretStore store = wxSecretStore::GetDefault();
        if (store.IsOk()) {
            store.Save(PROJECT_NAME, username, password);
        }
    }

    this->config->Write("/paths/lora", this->m_lora_dir->GetPath());
    this->config->Write("/paths/model", this->m_model_dir->GetPath());
    this->config->Write("/paths/vae", this->m_vae_dir->GetPath());
    this->config->Write("/paths/embedding", this->m_embedding_dir->GetPath());
    this->config->Write("/paths/taesd", this->m_taesd_dir->GetPath());
    this->config->Write("/paths/esrgan", this->m_esrgan_dir->GetPath());
    this->config->Write("/paths/controlnet", this->m_controlnet_dir->GetPath());
    this->config->Write("/paths/presets", this->m_presets_dir->GetPath());
    this->config->Write("/keep_model_in_memory", this->m_keep_model_in_memory->GetValue());
    this->config->Write("/save_all_image", this->m_save_all_image->GetValue());
    this->config->Write("/n_threads", this->m_threads->GetValue());
    this->config->Write("/paths/output", this->m_images_output->GetPath());
    this->config->Write("/image_quality", this->m_image_quality->GetValue());
    this->config->Write("/png_compression_level", this->m_png_compression->GetValue());
    this->config->Write("/image_type", this->m_image_type->GetStringSelection());
    this->config->Write("/show_notification", this->m_show_notifications->GetValue());
    this->config->Write("/notification_timeout", this->m_notification_timeout->GetValue());
    this->config->Write("/enable_civitai", this->m_enableCivitai->GetValue());
    this->config->Write("/output_filename_format", this->m_output_filename_format->GetValue());
    this->config->Write("/autogen_hash", this->m_autogen_hash->GetValue());

    this->cfg->lora                   = this->m_lora_dir->GetPath().utf8_string();
    this->cfg->model                  = this->m_model_dir->GetPath().utf8_string();
    this->cfg->vae                    = this->m_vae_dir->GetPath().utf8_string();
    this->cfg->embedding              = this->m_embedding_dir->GetPath().utf8_string();
    this->cfg->taesd                  = this->m_taesd_dir->GetPath().utf8_string();
    this->cfg->esrgan                 = this->m_esrgan_dir->GetPath().utf8_string();
    this->cfg->controlnet             = this->m_controlnet_dir->GetPath().utf8_string();
    this->cfg->presets                = this->m_presets_dir->GetPath().utf8_string();
    this->cfg->keep_model_in_memory   = this->m_keep_model_in_memory->GetValue();
    this->cfg->save_all_image         = this->m_save_all_image->GetValue();
    this->cfg->n_threads              = this->m_threads->GetValue();
    this->cfg->output                 = this->m_images_output->GetPath().utf8_string();
    this->cfg->image_quality          = this->m_image_quality->GetValue();
    this->cfg->png_compression_level  = this->m_png_compression->GetValue();
    this->cfg->image_type             = sd_gui_utils::image_types_str_reverse.at(this->m_image_type->GetStringSelection());
    this->cfg->show_notifications     = this->m_show_notifications->GetValue();
    this->cfg->notification_timeout   = this->m_notification_timeout->GetValue();
    this->cfg->enable_civitai         = this->m_enableCivitai->GetValue();
    this->cfg->output_filename_format = this->m_output_filename_format->GetValue();
    this->cfg->auto_gen_hash          = this->m_autogen_hash->GetValue();

    auto language = this->locales[this->m_language->GetSelection()];
    this->cfg->language = language;
    this->config->Write("/language", wxString::FromUTF8Unchecked(language));
    this->config->Flush();
    this->Close();
}

void MainWindowSettings::InitConfig() {
    this->m_staticNumberOfCores->SetLabel(wxString::Format("%d", cores()));

    wxString username = "civitai_api_key";
    wxSecretValue password;

    wxSecretStore store = wxSecretStore::GetDefault();

    if (store.IsOk() && store.Load(PROJECT_NAME, username, password)) {
        this->m_civitai_api_key->SetValue(password.GetAsString());
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
    this->m_png_compression->SetValue(this->cfg->png_compression_level);
    this->m_image_type->Select((int)this->cfg->image_type);
    this->m_show_notifications->SetValue(this->cfg->show_notifications);
    this->m_notification_timeout->SetValue(this->cfg->notification_timeout);
    this->m_enableCivitai->SetValue(this->cfg->enable_civitai);
    this->m_output_filename_format->SetValue(this->cfg->output_filename_format);
    this->m_autogen_hash->SetValue(this->cfg->auto_gen_hash);

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

    auto systemLocale = wxUILocale::GetSystemLocaleId().GetLanguage();


    auto langs = tr->GetAvailableTranslations("stablediffusiongui");

    auto required_locale = this->cfg->language != systemLocale.utf8_string() && this->cfg->language.empty() == false ? this->cfg->language : systemLocale;

    int selected = 0;
    int counter  = 0;

    std::map<wxString, wxString> _locales;  // to avoid duplicates

    for (const auto lang : langs) {
        auto langInfo = wxUILocale::FindLanguageInfo(lang);
        if (langInfo != nullptr && langInfo->DescriptionNative.empty() == false && langInfo->CanonicalName.empty() == false) {
            _locales[langInfo->CanonicalName.utf8_string()] = langInfo->DescriptionNative.utf8_string();
        }
    }

    for (const auto _locale : _locales) {
        this->m_language->Append(_locale.second);
        if (_locale.first == required_locale) {
            selected = counter;
        }
        this->locales[counter] = _locale.first.utf8_string();
        counter++;
    }
    this->m_language->SetSelection(selected);
}