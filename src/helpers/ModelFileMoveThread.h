class FileMoveThread : public wxThread {
public:
    FileMoveThread(const wxString& source, const wxString& dest, wxEvtHandler* handler)
        : wxThread(wxTHREAD_JOINABLE), m_source(source), m_dest(dest), m_handler(handler) {}

protected:
    ExitCode Entry() override {
        const size_t bufferSize = 1024 * 1024; // 1 MB
        char buffer[bufferSize];

        std::ifstream inFile(m_source.ToStdString(), std::ios::binary);
        std::ofstream outFile(m_dest.ToStdString(), std::ios::binary);

        if (!inFile || !outFile) {
            PostProgressEvent(0, wxT("Failed to open files."));
            return (ExitCode)0;
        }

        size_t totalBytes = 0;
        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        while (!TestDestroy() && inFile) {
            inFile.read(buffer, bufferSize);
            std::streamsize bytesRead = inFile.gcount();
            outFile.write(buffer, bytesRead);

            totalBytes += bytesRead;
            int progress = static_cast<int>(100.0 * totalBytes / fileSize);

            wxThreadEvent* evt = new wxThreadEvent(wxEVT_THREAD, wxID_ANY);
            evt->SetInt(progress);
            m_handler->QueueEvent(evt);
        }

        return (ExitCode)0;
    }

private:
    wxString m_source;
    wxString m_dest;
    wxEvtHandler* m_handler;

    void PostProgressEvent(int progress, const wxString& message) {
        wxThreadEvent evt(wxEVT_THREAD, wxID_ANY);
        evt.SetInt(progress);
        evt.SetString(message);
        m_handler->QueueEvent(evt.Clone());
    }
};