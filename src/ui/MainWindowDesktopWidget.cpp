#include "MainWindowDesktopWidget.h"
#include "embedded_files/widget_bg.png.h"

MainWindowDesktopWidget::MainWindowDesktopWidget(wxWindow* parent)
    : DesktopWidget(parent) {
    this->dragging = false;

    this->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    this->SetBackgroundColour(wxColour(0, 0, 0, 0));
    

    this->m_background = widget_bg_png_to_wx_bitmap();
    this->m_background.UseAlpha();
    auto image = this->m_background.ConvertToImage();

    wxRegion region;
    int width  = this->m_background.GetWidth();
    int height = this->m_background.GetHeight();

    // Pixel-by-pixel átlátszósági maszk létrehozása
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char alpha = image.GetAlpha(x, y);
            if (alpha > 0)
            {
                region.Union(x, y, 1, 1);  // Pixel hozzáadása a régióhoz
            }
        }
    }

    // Alak beállítása a widgeten
    this->SetShape(region);

    parent->Bind(wxEVT_THREAD, &MainWindowDesktopWidget::OnThreadMessage, this);
}
void MainWindowDesktopWidget::OnThreadMessage(wxThreadEvent& e) {
    if (e.GetSkipped() == false) {
        e.Skip();
    }
    if (e.GetId() == 9999) {
        return;
    }
    auto msg = e.GetString().utf8_string();

    std::string token                      = msg.substr(0, msg.find(":"));
    std::string content                    = msg.substr(msg.find(":") + 1);
    sd_gui_utils::ThreadEvents threadEvent = (sd_gui_utils::ThreadEvents)std::stoi(token);

    if (threadEvent == sd_gui_utils::ThreadEvents::QUEUE) {
        QM::QueueEvents event = (QM::QueueEvents)std::stoi(content);

        std::shared_ptr<QM::QueueItem> item = e.GetPayload<std::shared_ptr<QM::QueueItem>>();

        size_t stepsSum  = 0;  // item->params.sample_steps * item->params.batch_count;
        size_t stepsDone = 0;  // item->stats.time_per_step.size();
        wxString steps   = wxEmptyString;

        if (event == QM::QueueEvents::ITEM_MODEL_HASH_START || event == QM::QueueEvents::ITEM_MODEL_HASH_UPDATE) {
            stepsSum  = item->hash_fullsize;
            stepsDone = item->hash_progress_size;

            if (stepsDone > stepsSum) {
                stepsDone = stepsSum;
            }

            this->m_currentProgress->SetRange(stepsSum);
            this->m_currentProgress->SetValue(stepsDone);
            steps = wxString::Format("%s/%s",
                                     wxFileName::GetHumanReadableSize((wxULongLong)stepsDone),
                                     wxFileName::GetHumanReadableSize((wxULongLong)stepsSum));
        }
        if (event == QM::QueueEvents::ITEM_MODEL_HASH_DONE || event == QM::QueueEvents::ITEM_UPDATED || event == QM::QueueEvents::ITEM_FINISHED) {
            stepsSum  = (size_t)(item->params.sample_steps * item->params.batch_count);
            stepsDone = item->stats.time_per_step.size();

            if (stepsSum < stepsDone) {
                stepsSum  = item->steps;
                stepsDone = item->step > stepsSum ? stepsSum : item->step;
            }

            steps = wxString::Format("%zu/%zu", stepsDone, stepsSum);

            this->m_currentProgress->SetRange(stepsSum);
            this->m_currentProgress->SetValue(stepsDone);
        }

        // wxString step = wxString::Format("%d/%d", stepsDone, stepsSum);

        this->m_currentStatus->SetLabel(wxString::Format(_("Current job: %s %s %s"),
                                                         modes_str[item->mode],
                                                         wxGetTranslation(QM::QueueStatus_GUI_str.at(item->status)),
                                                         steps));
    }
}

void MainWindowDesktopWidget::OnMouseEnter(wxMouseEvent& event) {
}

void MainWindowDesktopWidget::OnMouseLeave(wxMouseEvent& event) {
}
void MainWindowDesktopWidget::OnMouseLeftDown(wxMouseEvent& event) {
}

void MainWindowDesktopWidget::OnMouseLeftUp(wxMouseEvent& event) {
}

void MainWindowDesktopWidget::OnMouseMotion(wxMouseEvent& event) {
    if (event.Dragging() && event.LeftIsDown()) {
        wxPoint mouseOnScreen = wxGetMousePosition();

        // Ha a dragging még nem kezdődött meg, állítsuk be a kezdeti értékeket
        if (!this->dragging) {
            this->dragging = true;

            wxPoint windowScreenPos = this->GetScreenPosition();
            m_delta                 = wxPoint(mouseOnScreen.x - windowScreenPos.x, mouseOnScreen.y - windowScreenPos.y);

            std::cout << "Start dragging. Delta: " << m_delta.x << ", " << m_delta.y << std::endl;
        }

        // Mozgás kiszámítása és ablak pozíciójának frissítése
        wxPoint newPosition = wxPoint(mouseOnScreen.x - m_delta.x, mouseOnScreen.y - m_delta.y);
        this->Move(newPosition);
    } else {
        this->dragging = false;
    }
}

void MainWindowDesktopWidget::OnLeftMouseDClick(wxMouseEvent& event) {
    auto p = static_cast<MainWindowUI*>(this->m_parent);
    // p->RequestUserAttention();
    p->Raise();
}

void MainWindowDesktopWidget::OnWidgetPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        //        gc->SetBrush(wxBrush(wxColour(255, 0, 0, 27)));
        //        gc->DrawRectangle(0, 0, this->GetSize().GetWidth(), this->GetSize().GetHeight());

        if (m_background.IsOk()) {
            gc->DrawBitmap(m_background, 0, 0, m_background.GetWidth(), m_background.GetHeight());
        }
        delete gc;
    }
}