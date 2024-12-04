#include "MainWindowDesktopWidget.h"

MainWindowDesktopWidget::MainWindowDesktopWidget(wxWindow* parent)
    : DesktopWidget(parent) {
    this->dragging = false;
    parent->Bind(wxEVT_THREAD, &MainWindowDesktopWidget::OnThreadMessage, this);
}
void MainWindowDesktopWidget::OnThreadMessage(wxThreadEvent& e) {
    if (e.GetSkipped() == false) {
        e.Skip();
    }
    auto msg = e.GetString().utf8_string();

    std::string token                      = msg.substr(0, msg.find(":"));
    std::string content                    = msg.substr(msg.find(":") + 1);
    sd_gui_utils::ThreadEvents threadEvent = (sd_gui_utils::ThreadEvents)std::stoi(token);

    if (threadEvent == sd_gui_utils::ThreadEvents::QUEUE) {
        QM::QueueEvents event = (QM::QueueEvents)std::stoi(content);

        std::shared_ptr<QM::QueueItem> item = e.GetPayload<std::shared_ptr<QM::QueueItem>>();
        
        this->m_currentStatus->SetLabel(wxString::Format(_("Current job: %s %s"), modes_str[item->mode], wxGetTranslation(QM::QueueStatus_GUI_str.at(item->status))));
        
        if (event == QM::QueueEvents::ITEM_MODEL_HASH_START || event == QM::QueueEvents::ITEM_MODEL_HASH_UPDATE) {

            this->m_currentProgress->SetRange(item->hash_fullsize);
            this->m_currentProgress->SetValue(item->hash_progress_size > item->hash_fullsize ? item->hash_fullsize : item->hash_progress_size);
        }
        if (event == QM::QueueEvents::ITEM_MODEL_HASH_DONE) {
            unsigned int stepsSum  = item->params.sample_steps * item->params.batch_count;
            unsigned int stepsDone = item->stats.time_per_step.size();

            // upscaler and the tieled vae will generate more steps after diffusion
            if (stepsSum < stepsDone) {
                stepsSum  = item->steps;
                stepsDone = item->step;
            }
            this->m_currentProgress->SetRange(stepsSum);
            this->m_currentProgress->SetValue(stepsDone > stepsSum ? stepsSum : stepsDone);
        }

        if (event == QM::QueueEvents::ITEM_UPDATED) {
            unsigned int stepsSum  = item->params.sample_steps * item->params.batch_count;
            unsigned int stepsDone = item->stats.time_per_step.size();

            // upscaler and the tieled vae will generate more steps after diffusion
            if (stepsSum < stepsDone) {
                stepsSum  = item->steps;
                stepsDone = item->step;
            }
            this->m_currentProgress->SetRange(stepsSum);
            this->m_currentProgress->SetValue(stepsDone > stepsSum ? stepsSum : stepsDone);
        }

        if (event == QM::QueueEvents::ITEM_FINISHED) {
            unsigned int stepsSum  = item->params.sample_steps * item->params.batch_count;
            unsigned int stepsDone = item->stats.time_per_step.size();

            // upscaler and the tieled vae will generate more steps after diffusion
            if (stepsSum < stepsDone) {
                stepsSum  = item->steps;
                stepsDone = item->step;
            }
            this->m_currentProgress->SetRange(stepsSum);
            this->m_currentProgress->SetValue(stepsDone > stepsSum ? stepsSum : stepsDone);
        }
    }
}
void MainWindowDesktopWidget::OnClose(wxCloseEvent& event) {
    // this->RemoveEventHandler(this->GetEventHandler());
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
    //p->RequestUserAttention();
    p->Raise();
}