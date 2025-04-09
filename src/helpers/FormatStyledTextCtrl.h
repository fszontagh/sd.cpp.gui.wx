#ifndef STYLEWXSTYLEDTEXTCTRL_H
#define STYLEWXSTYLEDTEXTCTRL_H

#include <md4c-html.h>

namespace sd_gui_utils {

    inline static std::string formatTimestamp(uint64_t timestamp) {
        std::time_t time = static_cast<std::time_t>(timestamp / 1000);
        std::tm* tm_info = std::localtime(&time);
        std::ostringstream oss;
        oss << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    inline static wxString GenerateSystemCSS() {
        wxColour bgColour        = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
        wxColour textColour      = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        wxColour borderColour    = wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVEBORDER);
        wxColour highlightColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);

        // Helper function to ensure valid RGB values
        auto validColour = [](const wxColour& colour) {
            return (colour.IsOk()) ? colour : wxColour(0, 0, 0);  // Fallback to black if invalid
        };

        bgColour        = validColour(bgColour);
        textColour      = validColour(textColour);
        borderColour    = validColour(borderColour);
        highlightColour = validColour(highlightColour);

        wxString css;
        css << "* {\n"
            << "}\n"
            << "html, body {\n"
            << "    padding: 0;\n"
            << "    margin: 0;\n"
            << "    background-color: rgb(" << bgColour.GetRed() << ", " << bgColour.GetGreen() << ", " << bgColour.GetBlue() << ");\n"
            << "    color: rgb(" << textColour.GetRed() << ", " << textColour.GetGreen() << ", " << textColour.GetBlue() << ");\n"
            << "    font-size: 12px;\n"
            << "    font-family: Arial, sans-serif;\n"
            << "}\n"
            << ".chat-container {\n"
            << "    display: block;\n"
            << "    width: 100%;\n"
            << "    height: 100%;\n"
            << "    padding: 0;\n"
            << "    margin: 0;\n"
            << "}\n"
            << ".message {\n"
            << "    padding: 8px 12px;\n"
            << "    margin: 5px 0;\n"
            << "    border-radius: 5px;\n"
            << "    max-width: 80%;\n"
            << "    min-width: 30%;\n"
            << "    display: block;\n"
            << "    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);\n"
            << "    clear: both;\n"
            << "}\n"
            << ".assistant {\n"
            << "    text-align: left;\n"
            << "    float: left;\n"
            << "}\n"
            << ".user {\n"
            << "    text-align: right;\n"
            << "    float: right;\n"
            << "}\n"
            << ".system, .thinking {\n"
            << "    text-align: center;\n"
            << "    margin: 5px auto;\n"
            << "}\n"
            << "pre> code {\n"
            << "    background-color: rgb(" << highlightColour.GetRed() << ", " << highlightColour.GetGreen() << ", " << highlightColour.GetBlue() << ");\n"
            << "    border: 1px solid rgb(" << borderColour.GetRed() << ", " << borderColour.GetGreen() << ", " << borderColour.GetBlue() << ");\n"
            << "    padding: 4px;\n"
            << "    display: inline-block;\n"
            << "    border-radius: 8px;\n"
            << "    font-family: monospace;\n"
            << "    word-wrap: normal;\n"
            << "}\n"
            << "hr {\n"
            << "    border: 1px solid rgb(" << borderColour.GetRed() << ", " << borderColour.GetGreen() << ", " << borderColour.GetBlue() << ");\n"
            << "}\n"
            << "input, select, textarea {\n"
            << "    background-color: rgb(" << bgColour.GetRed() << ", " << bgColour.GetGreen() << ", " << bgColour.GetBlue() << ");\n"
            << "    color: rgb(" << textColour.GetRed() << ", " << textColour.GetGreen() << ", " << textColour.GetBlue() << ");\n"
            << "    border: 1px solid rgb(" << borderColour.GetRed() << ", " << borderColour.GetGreen() << ", " << borderColour.GetBlue() << ");\n"
            << "}\n"
            << "input[type=checkbox] {\n"
            << "    accent-color: rgb(" << highlightColour.GetRed() << ", " << highlightColour.GetGreen() << ", " << highlightColour.GetBlue() << ");\n"
            << "}\n";

        return css;
    }

    inline static wxString removeHTMLTags(const wxString& input) {
        wxString result = input;
        wxRegEx tagPattern("(<[^>]*>)|(&[^;]*;)|([^<>]*)");
        wxString replacementString = wxEmptyString;
        tagPattern.ReplaceAll(&result, replacementString);
        return result;
    }

    inline static wxString nl2br(const wxString& input) {
        wxString output = input;
        output.Replace(wxString("\n"), "<br/>", true);
        return output;
    }

    inline static wxString ConvertMarkdownToHtml(const wxString& markdown) {
        const std::string md_string = std::string(markdown.utf8_str().data(), markdown.length());
        std::string html_output;

        int result = md_html(
            markdown.c_str(), markdown.size(),
            [](const MD_CHAR* data, MD_SIZE size, void* userdata) {
                std::string* output = static_cast<std::string*>(userdata);
                output->append(data, size);
            },
            &html_output, MD_DIALECT_GITHUB, 0);

        return wxString(html_output.data(), html_output.size());
    }

    inline static void ConfigureTextCtrl(wxStyledTextCtrl* textCtrl) {
        if (!textCtrl) {
            return;
        }

        textCtrl->SetWrapMode(wxSTC_WRAP_WHITESPACE);
        textCtrl->StyleSetBackground(wxSTC_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        textCtrl->StyleSetForeground(wxSTC_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        textCtrl->SetCaretForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

        struct StyleConfig {
            sd_gui_utils::GuiPromptStyles style;
            wxColour color;
            wxFont font;

            StyleConfig(sd_gui_utils::GuiPromptStyles s, const wxColour& c, const wxFont& f)
                : style(s), color(c), font(f) {}
        };

        wxColour loraColor(50, 100, 150);
        wxColour embeddingColor(50, 168, 143);
        wxColour notFoundColor(245, 12, 70);
        wxColour othersColor(220, 220, 50);

        auto defaultFont  = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        auto notFoundFont = defaultFont;
        notFoundFont.SetStrikethrough(true);

        std::vector<StyleConfig> styles{
            StyleConfig(sd_gui_utils::GuiPromptStyles::STYLE_OTHERS, othersColor, defaultFont),
            StyleConfig(sd_gui_utils::GuiPromptStyles::STYLE_LORA, loraColor, defaultFont),
            StyleConfig(sd_gui_utils::GuiPromptStyles::STYLE_EMBEDDING, embeddingColor, defaultFont),
            StyleConfig(sd_gui_utils::GuiPromptStyles::STYLE_LORA_NOT_FOUND, notFoundColor, notFoundFont)};

        for (const auto& style : styles) {
            textCtrl->StyleSetFont(+style.style, style.font);
            textCtrl->StyleSetBackground(+style.style, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
            textCtrl->StyleSetForeground(+style.style, style.color);
        }
    }

    inline static wxString EscapeJavaScriptString(const wxString& input) {
        wxString output = input;
        output.Replace("\\", "\\\\");  // Escape backslash
        output.Replace("'", "\\'");    // Escape single quote
        output.Replace("\n", "\\n");   // Escape newline
        output.Replace("\r", "\\r");   // Escape carriage return
        return output;
    }

    inline static void UpdateChatMessage(wxWebView* webView, const wxString& message, uint64_t message_id) {
        if (!webView) {
            return;
        }
        wxString safeMessage = EscapeJavaScriptString(message);
        wxString script      = wxString::Format("updateMessageContent(%" PRIi64 ", '%s');", message_id, safeMessage);
        webView->RunScriptAsync(script);  // we dont care if it success or not, just run it... The IE backend does not support async script execution.
    }

    inline static void AddMessage(wxWebView* webView, const wxString& role, const wxString& content, uint64_t message_id) {
        if (!webView) {
            return;
        }
        wxString safeContent = EscapeJavaScriptString(content);
        wxString script      = wxString::Format("addMessage('%s', '%s', %" PRIi64 ");", role, safeContent, message_id);
        if (!webView->RunScript(script)) {
            std::cout << "Script execution failed: " << script.ToStdString() << std::endl;
        }
    }

    inline static wxString ChatMessageTemplate() {
        // wxString nMessage = sd_gui_utils::ConvertMarkdownToHtml(message);
        //  replace all newline to <br/>\n
        //  nMessage.Replace("\n", "<br/>\n", true);
        const wxString script = R"(<script type="text/javascript">

                    function addMessage(role, content, id) {
                        const chatContainer = document.querySelector('.chat-container');
                        const newMessage = document.createElement('div');
                        newMessage.classList.add('message', role);
                        newMessage.setAttribute('data-id', id);
                        newMessage.innerHTML = `
                            <div class="title"></div>
                            <div class="content">${content}</div>
                        `;
                        chatContainer.appendChild(newMessage);
                        newMessage.scrollIntoView({ behavior: 'smooth', block: 'end' });
                    }

                    function updateMessageContent(id, newContent) {
                        const message = document.querySelector(`.message[data-id="${id}"]`);
                        if (message) {
                            message.querySelector('.content').innerHTML = newContent;
                            message.scrollIntoView({ behavior: 'smooth', block: 'end' });
                        }
                    }

                    window.addEventListener('keydown', function(event) {
                        if ((event.ctrlKey && event.key === 'r') || event.key === 'F5') {
                            event.preventDefault();
                        }
                    });

                </script>)";
        return wxString::Format("<!DOCTYPE html>\n<html>\n<head>\n<style type=\"text/css\">\n%s</style>\n\n%s\n</head>\n<body>\n<div class=\"chat-container\"></div></body>\n</html>", GenerateSystemCSS().c_str(), script);
    }

};

#endif  // STYLEWXSTYLEDTEXTCTRL_H