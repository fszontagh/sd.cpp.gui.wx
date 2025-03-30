#ifndef STYLEWXSTYLEDTEXTCTRL_H
#define STYLEWXSTYLEDTEXTCTRL_H

#include "cmark-gfm-core-extensions.h"
#include "cmark-gfm-extension_api.h"
#include "cmark-gfm/registry.h"

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
            << "    font-size: 12px;\n"
            << "    background-color: rgb(" << bgColour.GetRed() << ", " << bgColour.GetGreen() << ", " << bgColour.GetBlue() << ");\n"
            << "    color: rgb(" << textColour.GetRed() << ", " << textColour.GetGreen() << ", " << textColour.GetBlue() << ");\n"
            << "    font-family: sans-serif;\n"
            << "    margin: 0; padding:0;\n"
            << "}\n"
            << "html,body {\n"
            << "    padding: 0;\n"
            << "    margin: 0;\n"
            << "}\n"
            << "code, pre {\n"
            << "    background-color: rgb(" << highlightColour.GetRed() << ", " << highlightColour.GetGreen() << ", " << highlightColour.GetBlue() << ");\n"
            << "    padding: 5px;\n"
            << "    border-radius: 3px;\n"
            << "    font-family: monospace;\n"
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

    inline static wxString ConvertMarkdownToHtml(const wxString& markdown) {
        const std::string md_string = std::string(markdown.utf8_str().data(), markdown.length());

        const int options = CMARK_OPT_DEFAULT | CMARK_OPT_UNSAFE | CMARK_OPT_SMART | CMARK_OPT_GITHUB_PRE_LANG | CMARK_OPT_LIBERAL_HTML_TAG | CMARK_OPT_FOOTNOTES | CMARK_OPT_TABLE_PREFER_STYLE_ATTRIBUTES | CMARK_OPT_FULL_INFO_STRING;

        cmark_parser* parser = cmark_parser_new(options);

        cmark_gfm_core_extensions_ensure_registered();
        const char* extensions[] = {
            "table",
            "strikethrough",
            "autolink",
            "tagfilter",
            "tasklist",
        };

        for (const char* ext_name : extensions) {
            cmark_syntax_extension* ext = cmark_find_syntax_extension(ext_name);
            if (ext) {
                cmark_parser_attach_syntax_extension(parser, ext);
            } else {
                std::cerr << "No ext: " << ext_name << std::endl;
            }
        }

        cmark_node* root = cmark_parse_document(md_string.c_str(), md_string.size(), options);
        char* html       = cmark_render_html(root, options, cmark_parser_get_syntax_extensions(parser));

        wxString wxHtmlOutput = wxString::FromUTF8(html);

        cmark_node_free(root);

        free(html);

        cmark_parser_free(parser);
        return wxHtmlOutput;
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

    inline static wxString ChatMessageTemplate(const wxString& message) {
        wxString nMessage = sd_gui_utils::ConvertMarkdownToHtml(message);
        // replace all newline to <br/>\n
        nMessage.Replace("\n", "<br/>\n", true);
        const wxString script = R"(<script>
                    document.addEventListener('contextmenu', function(event) {
                        event.preventDefault();
                    });
                    window.addEventListener('keydown', function(event) {
                        if ((event.ctrlKey && event.key === 'r') || event.key === 'F5') {
                            event.preventDefault();
                        }
                    });
                    window.onload = function() {
                        history.pushState(null, null, window.location.href);
                        window.onpopstate = function () {
                            history.pushState(null, null, window.location.href);
                        };
                    };
                </script>)";
        return wxString::Format("<!DOCTYPE html>\n<html>\n<head>\n<style type=\"text/css\">\n%s</style>\n\n%s\n</head>\n<body>%s<!--Original msg: \n\n%s--></body>\n</html>", GenerateSystemCSS().c_str(), script, nMessage, message);
    }

};

#endif  // STYLEWXSTYLEDTEXTCTRL_H