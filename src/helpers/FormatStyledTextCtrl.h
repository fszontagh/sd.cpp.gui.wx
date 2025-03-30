#ifndef STYLEWXSTYLEDTEXTCTRL_H
#define STYLEWXSTYLEDTEXTCTRL_H

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
            << "    background-color: rgb(" << bgColour.GetRed() << ", " << bgColour.GetGreen() << ", " << bgColour.GetBlue() << ");\n"
            << "    color: rgb(" << textColour.GetRed() << ", " << textColour.GetGreen() << ", " << textColour.GetBlue() << ");\n"
            << "    font-family: sans-serif;\n"
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
        wxString html = markdown;

        // 1. Convert headers (Markdown: # Header, ## Subheader)
        wxRegEx headerRegex(wxT("^(#+) (.+)$"));
        wxString result;
        result = markdown;
        wxString processed;
        wxString line;
        wxString finalHtml;

        wxArrayString lines;
        wxStringTokenizer tokenizer(result, wxT("\n"));

        while (tokenizer.HasMoreTokens()) {
            line = tokenizer.GetNextToken();

            if (headerRegex.Matches(line)) {
                wxString headerLevel = headerRegex.GetMatch(line, 1);
                wxString headerText  = headerRegex.GetMatch(line, 2);
                processed            = wxString::Format("<h%d>%s</h%d>", headerLevel.Length(), headerText, headerLevel.Length());
            } else {
                processed = line;
            }

            lines.Add(processed);
        }

        // Combine lines to the final html
        finalHtml = wxJoin(lines, '\n');

        // 2. Convert bold text (Markdown: **bold**)
        wxRegEx boldRegex(wxT("\\*\\*(.+?)\\*\\*"));
        boldRegex.ReplaceAll(&finalHtml, "<b>\\1</b>");

        // 3. Convert italic text (Markdown: *italic*)
        wxRegEx italicRegex(wxT("\\*(.+?)\\*"));
        italicRegex.ReplaceAll(&finalHtml, "<i>\\1</i>");

        // 4. Convert unordered lists (Markdown: - Item)
        wxRegEx listRegex(wxT("^\\s*- (.+)$"));
        listRegex.ReplaceAll(&finalHtml, "<ul><li>\\1</li></ul>");

        // 5. Convert ordered lists (Markdown: 1. Item)
        wxRegEx orderedListRegex(wxT("^\\s*\\d+\\. (.+)$"));
        orderedListRegex.ReplaceAll(&finalHtml, "<ol><li>\\1</li></ol>");

        // 6. Convert links (Markdown: [Link Text](url))
        wxRegEx linkRegex(wxT("\\[([^\]]+)\\]\\(([^)]+)\\)"));
        linkRegex.ReplaceAll(&finalHtml, "<a href=\"\\2\">\\1</a>");

        // 7. Convert blockquotes (Markdown: > Blockquote)
        wxRegEx blockquoteRegex(wxT("^> (.+)$"));
        wxArrayString blockquoteLines;
        tokenizer.SetString(finalHtml);
        while (tokenizer.HasMoreTokens()) {
            line = tokenizer.GetNextToken();
            if (blockquoteRegex.Matches(line)) {
                blockquoteLines.Add(wxString::Format("<blockquote>%s</blockquote>", blockquoteRegex.GetMatch(line, 1)));
            } else {
                blockquoteLines.Add(line);  // Non-blockquote lines are added without change
            }
        }

        // Combine blockquote lines into final html
        finalHtml = wxJoin(blockquoteLines, '\n');

        // 8. Convert code blocks (Markdown: ```code```)
        wxRegEx codeBlockRegex(wxT("```([^`]+)```"));
        codeBlockRegex.ReplaceAll(&finalHtml, "<pre><code>$1</code></pre>");
        // 9. Convert horizontal rules (Markdown: ---)
        wxRegEx horizontalRuleRegex(wxT("^---$"));
        horizontalRuleRegex.ReplaceAll(&finalHtml, "<hr/>");
        // 10. Convert inline code (Markdown: `code`)
        wxRegEx inlineCodeRegex(wxT("`([^`]+)`"));
        inlineCodeRegex.ReplaceAll(&finalHtml, "<code>$1</code>");
        // 11. Convert line breaks (Markdown: \n)
        wxRegEx lineBreakRegex(wxT("\\n"));
        lineBreakRegex.ReplaceAll(&finalHtml, "<br/>");

        return finalHtml;
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
        return wxString::Format("<!DOCTYPE html>\n<html>\n<head>\n<style type=\"text/css\">%s</style>\n\n%s\n</head>\n<body>%s</body>\n</html>", GenerateSystemCSS().c_str(), script, message);
    }

};

#endif  // STYLEWXSTYLEDTEXTCTRL_H