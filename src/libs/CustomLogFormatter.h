#ifndef __CUSTOM_LOG_FORMATTER__
#define __CUSTOM_LOG_FORMATTER__
#include <ctime>
class CustomLogFormatter : public wxLogFormatter {
public:
    wxString Format(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info) const override {
        wxString levelStr;
        switch (level) {
            case wxLOG_Info:
                levelStr = "INFO";
                break;
            case wxLOG_Error:
                levelStr = "ERROR";
                break;
            case wxLOG_Warning:
                levelStr = "WARNING";
                break;
            case wxLOG_Debug:
                levelStr = "DEBUG";
                break;
            default:
                levelStr = "LOG";
                break;
        }

        return wxString::Format("[%s]: %s", levelStr, msg);
    }
};

#endif  // __CUSTOM_LOG_FORMATTER__