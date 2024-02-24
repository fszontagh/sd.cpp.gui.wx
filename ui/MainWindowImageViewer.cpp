#include "MainWindowImageViewer.h"

MainWindowImageViewer::MainWindowImageViewer(wxWindow *parent)
    : ImageViewer(parent)
{

    auto bitmap = app_png_to_wx_bitmap();

    wxIcon icon;
    icon.CopyFromBitmap(bitmap);
    this->SetIcon(icon);
    wxPersistentRegisterAndRestore(this, this->GetName());
}

void MainWindowImageViewer::onImageViewClick(wxMouseEvent &event)
{
    // TODO: Implement onImageViewClick
}

void MainWindowImageViewer::SetData(QM::QueueItem item)
{
    this->myItem = item;
    bool first = true;
    for (auto img : item.images)
    {
        if (std::filesystem::exists(img))
        {
            wxImage *wimg = new wxImage();
            wimg->LoadFile(img);

            auto resized = sd_gui_utils::ResizeImageToMaxSize(*wimg, 100, 100);

            wxStaticBitmap *bitmap = new wxStaticBitmap(this->m_scrolledWindow2, wxID_ANY, wxNullBitmap, wxDefaultPosition, this->bitmapBaseSize, 0);
            bitmap->SetBackgroundColour(this->bitmapColor);
            bitmap->SetBitmap(resized);
            bitmap->SetSize(resized.GetSize());
            bitmap->SetClientData(wimg);
            bitmap->Bind(wxEVT_LEFT_UP, &MainWindowImageViewer::OnThumbnailClick, this);
            this->bitmaps.emplace_back(bitmap);
            this->thumbnails_container->Add(bitmap, 1, wxALL, 1);
            if (first)
            {
                first = false;
                this->SetImage(*wimg);
            }
        }
    }
    if (!item.params.control_image_path.empty() && std::filesystem::exists(item.params.control_image_path))
    {
        wxImage *ctrlwimg = new wxImage();
        ctrlwimg->LoadFile(item.params.control_image_path);

        auto ctrlresized = sd_gui_utils::ResizeImageToMaxSize(*ctrlwimg, 100, 100);

        wxStaticBitmap *ctrlbitmap = new wxStaticBitmap(this->m_scrolledWindow2, wxID_ANY, wxNullBitmap, wxDefaultPosition, this->bitmapBaseSize, 0);
        ctrlbitmap->SetBackgroundColour(this->bitmapColor);
        ctrlbitmap->SetBitmap(ctrlresized);
        ctrlbitmap->SetSize(ctrlresized.GetSize());
        ctrlbitmap->SetClientData(ctrlwimg);
        this->bitmaps.emplace_back(ctrlbitmap);
        ctrlbitmap->Bind(wxEVT_LEFT_UP, &MainWindowImageViewer::OnThumbnailClick, this);
        this->thumbnails_container->Add(ctrlbitmap, 0, wxALL, 1);
    }
    // input image if img2img was the job
    if (!item.initial_image.empty() && std::filesystem::exists(item.initial_image))
    {
        wxImage *initImage = new wxImage();
        initImage->LoadFile(item.initial_image);

        auto initImgResized = sd_gui_utils::ResizeImageToMaxSize(*initImage, 100, 100);

        wxStaticBitmap *initBitmap = new wxStaticBitmap(this->m_scrolledWindow2, wxID_ANY, wxNullBitmap, wxDefaultPosition, this->bitmapBaseSize, 0);
        initBitmap->SetBackgroundColour(this->bitmapColor);
        initBitmap->SetBitmap(initImgResized);
        initBitmap->SetSize(initImgResized.GetSize());
        initBitmap->SetClientData(initImage);
        this->bitmaps.emplace_back(initBitmap);
        initBitmap->Bind(wxEVT_LEFT_UP, &MainWindowImageViewer::OnThumbnailClick, this);
        this->thumbnails_container->Add(initBitmap, 0, wxALL, 1);
    }

    this->m_static_id->SetLabel(wxString::Format("%d", item.id));
    this->m_static_type->SetLabel(wxString(sd_gui_utils::modes_str[item.mode]));

    this->m_static_model->SetLabel(item.model);
    if (item.mode == QM::GenerationMode::UPSCALE)
    {

        this->m_static_resolution->SetLabel(wxString::Format("%dx upscale", item.upscale_factor));
    }
    else
    {
        this->m_static_resolution->SetLabel(wxString::Format("%dx%dpx", item.params.width, item.params.height));
    }

    // this->m_static_prompt->SetLabel(item.params.prompt);
    this->m_static_prompt->SetValue(item.params.prompt);
    this->m_static_negative_prompt->SetValue(item.params.negative_prompt);

    this->m_static_cfg_scale->SetLabel(wxString::Format("%.1f", item.params.cfg_scale));
    this->m_static_clip_skip->SetLabel(wxString::Format("%d", item.params.clip_skip));
    this->m_static_seed->SetLabel(wxString::Format("%lld", item.params.seed));
    this->m_static_steps->SetLabel(wxString::Format("%d", item.params.sample_steps));
    this->m_static_sampler->SetLabel(sd_gui_utils::sample_method_str[item.params.sample_method]);
    this->m_static_sheduler->SetLabel(sd_gui_utils::schedule_str[item.params.schedule]);
    this->m_static_started->SetLabel(sd_gui_utils::formatUnixTimestampToDate(item.created_at));
    this->m_static_finished->SetLabel(sd_gui_utils::formatUnixTimestampToDate(item.finished_at));
    this->m_static_batch->SetLabel(fmt::format("{}", item.params.batch_count));
    this->m_bitmap6->SetCursor(wxCURSOR_MAGNIFIER);
}

void MainWindowImageViewer::OnThumbnailClick(wxMouseEvent &event)
{
    wxStaticBitmap *bitmap = (wxStaticBitmap *)event.GetEventObject();
    wxImage *img = (wxImage *)bitmap->GetClientData();
    this->SetImage(*img);
}

void MainWindowImageViewer::SetImage(wxImage img)
{

    if (img.GetWidth() > img.GetHeight())
    {
        this->m_panel9->SetSize(768, this->m_panel9->GetSize().GetHeight());
        this->Layout();
    }

    this->m_bitmap6->SetBitmap(img);
    this->m_bitmap6->SetSize(img.GetSize());
    this->m_bitmap6->Center();
    this->m_bitmap6->Refresh();
    this->Refresh();
}
