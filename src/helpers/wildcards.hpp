#ifndef __SD_GUI_HELPERS_WILDCARDS__
#define __SD_GUI_HELPERS_WILDCARDS__
#include <vector>
#include <string>
#include <iostream>

namespace sd_gui_helpers
{
    class WildCards
    {
    public:
        static std::vector<std::string> extractWords(const std::string &input)
        {
            std::vector<std::string> words;

            size_t startPos = input.find('{');
            size_t endPos;
            while (startPos != std::string::npos)
            {
                endPos = input.find('}', startPos);
                if (endPos == std::string::npos)
                {
                    break; // If '}' is not found, break the loop
                }
                std::string wordSet = input.substr(startPos + 1, endPos - startPos - 1);
                size_t pipePos = wordSet.find('|');
                size_t prevPos = 0;
                while (pipePos != std::string::npos)
                {
                    std::string word = wordSet.substr(prevPos, pipePos - prevPos);
                    trim(word); // Remove leading and trailing spaces
                    if (!word.empty())
                    {
                        words.push_back(word);
                    }
                    prevPos = pipePos + 1;
                    pipePos = wordSet.find('|', prevPos);
                }
                std::string lastWord = wordSet.substr(prevPos);
                trim(lastWord); // Remove leading and trailing spaces
                if (!lastWord.empty())
                {
                    words.push_back(lastWord);
                }

                // Find the next occurrence of '{'
                startPos = input.find('{', endPos);
            }
            return words;
        }

    private:
        static void trim(std::string &str)
        {
            // Remove leading spaces
            size_t startPos = str.find_first_not_of(' ');
            if (startPos != std::string::npos)
            {
                str = str.substr(startPos);
            }

            // Remove trailing spaces
            size_t endPos = str.find_last_not_of(' ');
            if (endPos != std::string::npos)
            {
                str = str.substr(0, endPos + 1);
            }
        }
    };
}
#endif