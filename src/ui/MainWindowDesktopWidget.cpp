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

        size_t stepsSum  = 0;  // item->params.sample_steps * item->params.batch_count;
        size_t stepsDone = 0;  // item->stats.time_per_step.size();
        wxString steps         = wxEmptyString;

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

            steps = wxString::Format("%d/%d", stepsDone, stepsSum);

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
    // p->RequestUserAttention();
    p->Raise();
}