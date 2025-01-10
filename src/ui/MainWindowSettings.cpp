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
void MainWindowSettings::OnAddServer(wxCommandEvent& event) {
    auto host = this->m_AddServerHost->GetValue();
    int port;
    if (this->m_AddServerPort->GetValue().ToInt(&port) && host.empty() == false) {
        // check if server already exists
        if (this->cfg->ServerExist(host.utf8_string(), port) == true) {
            wxMessageBox(wxString::Format(_("Server %s:%d already exists"), host, port), "Error", wxICON_ERROR);
            return;
        }
        auto srv = new sd_gui_utils::sdServer(host.utf8_string(), port, this->m_parent->GetEventHandler());
        this->cfg->AddTcpServer(srv);
        this->AddRemoteServerToList(srv);
    }
}

void MainWindowSettings::onSave(wxCommandEvent& event) {
    // save secret
    wxString username = "civitai_api_key";

    if (!this->m_civitai_api_key->GetValue().empty()) {
        wxSecretValue password(this->m_civitai_api_key->GetValue());
        wxSecretStore store = wxSecretStore::GetDefault();
        if (store.IsOk()) {
            store.Save(wxString::Format(wxT("%s/CivitAiApiKey"), PROJECT_NAME), username, password);
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
    this->config->Write("/favorite_models_only", this->m_favorite_models_only->GetValue());

    wxString oldPath = this->config->GetPath();
    this->config->DeleteGroup("Server");
    this->config->SetPath("/Servers");
    for (size_t i = 0; i < this->cfg->servers.size(); ++i) {
        this->config->SetPath(wxString::Format("Server%zu", i));
        this->config->Write("Host", wxString::FromUTF8Unchecked(this->cfg->servers[i]->GetHost()));
        this->config->Write("Port", this->cfg->servers[i]->GetPort());
        this->config->Write("InternalId", this->cfg->servers[i]->GetInternalId());
        this->config->Write("Enabled", this->cfg->servers[i]->IsEnabled());
        this->config->Write("Id", wxString::FromUTF8Unchecked(this->cfg->servers[i]->GetId()));
        this->config->Write("Name", this->cfg->servers[i]->GetName());
        this->config->SetPath("..");
    }
    this->config->SetPath(oldPath);

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
    this->cfg->favorite_models_only   = this->m_favorite_models_only->GetValue();

    auto language       = this->locales[this->m_language->GetSelection()];
    this->cfg->language = language;
    this->config->Write("/language", wxString::FromUTF8Unchecked(language));
    this->config->Flush();
    this->Close();
}

void MainWindowSettings::InitConfig() {
    this->m_staticNumberOfCores->SetLabel(wxString::Format("%d", cores()));

    wxString username;
    wxSecretValue password;

    wxSecretStore store = wxSecretStore::GetDefault();

    if (store.IsOk() && store.Load(wxString::Format(wxT("%s/CivitAiApiKey"), PROJECT_NAME), username, password)) {
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
    this->m_favorite_models_only->SetValue(this->cfg->favorite_models_only);

    for (auto& srv : this->cfg->ListRemoteServers()) {
        this->AddRemoteServerToList(srv);
    }

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
void MainWindowSettings::AddRemoteServerToList(sd_gui_utils::sdServer* server) {
    this->checkboxUpdate = true;
    wxSecretStore store  = wxSecretStore::GetDefault();
    wxString username;
    wxSecretValue authkey;

    if (store.IsOk() && store.Load(server->GetsecretKeyName(), username, authkey)) {
        server->SetAuthKeyState(true);
    }

    wxVector<wxVariant> values;
    values.push_back(wxString::FromUTF8Unchecked(server->GetHost()));
    values.push_back(wxString::Format(wxT("%d"), server->GetPort()));
    values.push_back(authkey.GetAsString());
    values.push_back(wxGetTranslation((server->GetStatus())));
    // get last row
    auto row = this->m_serverList->GetItemCount();
    if (server->GetInternalId() == -1) {
        server->SetInternalId(row);
    }
    this->m_serverList->InsertItem(row, values, server->GetInternalId());
    values.clear();
    this->checkboxUpdate = false;
}

void MainWindowSettings::OnDeleteServer(wxCommandEvent& event) {
    wxDataViewItemArray selections;
    auto idx = this->m_serverList->GetSelections(selections);
    if (idx == 0) {
        return;
    }

    for (const auto item : selections) {
        auto internal_id = static_cast<int>(this->m_serverList->GetItemData(item));
        auto row         = this->m_serverList->ItemToRow(item);
        this->m_serverList->DeleteItem(row);
        this->cfg->RemoveTcpServer(internal_id);
    }
}

void MainWindowSettings::OnServerListStartEditing(wxDataViewEvent& event) {
    // std::cout << "OnServerListStartEditing" << std::endl;
};
void MainWindowSettings::OnServerListEditingStarted(wxDataViewEvent& event) {
    // std::cout << "OnServerListEditingStarted" << std::endl;
};
void MainWindowSettings::OnServerListItemValueChanged(wxDataViewEvent& event) {
    /*
        if (this->checkboxUpdate == true) {
            event.Skip();
            return;
        }
        auto col  = event.GetColumn();
        auto item = event.GetItem();
        auto row  = this->m_serverList->ItemToRow(item);

        if (col == ServerListColumns::SERVER_LIST_COLUMN_ENABLE) {
            auto value = event.GetValue().GetBool();
            for (auto& srv : this->cfg->servers) {
                if (srv.row == row &&  value == true) {
                    if (srv.authkey.empty() || srv.host.empty() || srv.port == 0) {
                        this->checkboxUpdate = true;
                        wxMessageDialog(this, _("Please fill all required fields before enabling the server")).ShowModal();
                        this->checkboxUpdate = false;
                        event.Skip();
                        return;
                    }
                    srv.enabled = value;
                }
            }
        }
        */
}
void MainWindowSettings::OnServerListEditingDone(wxDataViewEvent& event) {
    auto value = event.GetValue().GetString();
    if (value.empty()) {
        return;
    }
    auto col        = event.GetColumn();
    auto item       = event.GetItem();
    auto row        = this->m_serverList->ItemToRow(item);
    int internal_id = static_cast<int>(this->m_serverList->GetItemData(item));

    auto srv = this->cfg->GetTcpServer(internal_id);
    if (srv == nullptr) {
        return;
    }
    if (col == ServerListColumns::SERVER_LIST_COLUMN_HOST) {
        if (this->cfg->ServerExist(value.utf8_string(), srv->GetPort())) {
            wxMessageDialog(this, _("Server already exists")).ShowModal();
            this->m_serverList->SetValue(wxString::FromUTF8Unchecked(srv->GetHost()), row, col);
        }
        this->cfg->ServerChangeHost(internal_id, value.utf8_string());
    }

    if (col == ServerListColumns::SERVER_LIST_COLUMN_AUTH_KEY) {
        if (value.Length() != 64 && value.Length() != 0) {
            wxMessageDialog(this, _("Authkey must be 64 characters long")).ShowModal();
            this->m_serverList->SetValue(wxEmptyString, row, col);
            return;
        }
        wxSecretStore store  = wxSecretStore::GetDefault();
        wxString serviceName = wxString::Format(wxT("%s/%s_%d"), PROJECT_NAME, wxString::FromUTF8Unchecked(srv->GetHost()), srv->GetPort());
        wxString username    = "authkey";
        wxSecretValue authkey(value);
        if (store.IsOk()) {
            if (value.Length() == 0) {
                store.Delete(serviceName);
                return;
            } else {
                if (store.Save(serviceName, username, authkey) == true) {
                    srv->SetAuthKeyState(true);
                    return;
                }
            }
        }
        wxMessageDialog(this, _("Failed to save authkey")).ShowModal();
    }

    if (col == ServerListColumns::SERVER_LIST_COLUMN_PORT) {
        int newPort = 0;
        if (value.ToInt(&newPort) == true && newPort > 0 && newPort < 65536) {
            for (const auto& server : this->cfg->servers) {
                if (server->GetPort() == newPort && server->GetPort() == srv->GetPort() && server->GetInternalId() != internal_id) {
                    wxMessageDialog(this, _("Server already exists")).ShowModal();
                    this->m_serverList->SetValue(wxString::Format(wxT("%d"), srv->GetPort()), row, col);
                    return;
                }
            }
            this->cfg->ServerChangePort(internal_id, newPort);
            this->m_serverList->SetValue(wxString::Format(wxT("%d"), srv->GetPort()), row, col);
        }
    }
};

void MainWindowSettings::OnServerListSelectionChanged(wxDataViewEvent& event) {
    if (this->m_serverList->GetSelectedItemsCount() > 0) {
        this->m_deleteServer->Enable();
    } else {
        this->m_deleteServer->Disable();
    }

    if (this->m_serverList->GetSelectedItemsCount() == 1) {
        this->m_serverEnable->Enable();
        auto item        = event.GetItem();
        auto internal_id = static_cast<int>(this->m_serverList->GetItemData(item));
        for (auto& srv : this->cfg->ListRemoteServers()) {
            if (srv->GetInternalId() == internal_id) {
                this->m_serverEnable->SetValue(srv->IsEnabled());
                this->m_serverEnable->SetLabel(srv->IsEnabled() ? _("Disable") : _("Enable"));
                break;
            }
        }
    } else {
        this->m_serverEnable->Disable();
    }
}
void MainWindowSettings::OnServerEnableToggle(wxCommandEvent& event) {
    auto item = this->m_serverList->GetSelection();
    auto row  = this->m_serverList->ItemToRow(item);
    if (item.IsOk()) {
        auto internal_id = static_cast<int>(this->m_serverList->GetItemData(item));
        for (auto& srv : this->cfg->servers) {
            if (srv->GetInternalId() == internal_id) {
                if ((srv->GetAuthKeyState() == false || srv->GetHost().empty() || srv->GetPort() == 0) && this->m_serverEnable->GetValue() == true) {
                    this->m_serverEnable->SetValue(srv->IsEnabled());
                    wxMessageDialog(this, _("Please fill all required fields before enabling the server")).ShowModal();
                    return;
                }
                this->cfg->ServerEnable(internal_id, this->m_serverEnable->GetValue());
                ChangeRemoteServer(srv->GetStatus(), ServerListColumns::SERVER_LIST_COLUMN_STATUS, row);
                this->m_serverEnable->SetLabel(srv->IsEnabled() ? _("Disable") : _("Enable"));
                break;
            }
        }
    }
}
void MainWindowSettings::ChangeRemoteServer(const wxString& value, ServerListColumns col, int row) {
    this->m_serverList->SetValue(value, row, col);
}