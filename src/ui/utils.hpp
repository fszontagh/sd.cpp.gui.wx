#ifndef __MAINFRAME_HPP__UTILS
#define __MAINFRAME_HPP__UTILS

#include <filesystem>
#include <random>
#include "../helpers/civitai.hpp"
#include "../helpers/sd.hpp"
#include "../libs/bitmask_operators.h"


namespace sd_gui_utils {

    /**
     * \brief Splits a string into a vector of integers using a separator character.
     *
     * This function takes a string and a separator character, and splits the string
     * into a vector of integers using the separator character.
     *
     * \param str The string to be split.
     * \param sep The separator character to use.
     */
    inline std::vector<int> splitStr2int(const std::string& str, char sep) {
        std::vector<int> tokens;
        std::string token;
        for (char c : str) {
            if (c == sep) {
                if (!token.empty()) {
                    tokens.push_back(std::stoi(token));
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) {
            tokens.push_back(std::stoi(token));
        }
        return tokens;
    }

    typedef struct VoidHolder {
        void* p1;  // eventhandler
        void* p2;  // QM::QueueItem
        void* p3;  // Others...
    } VoidHolder;

    enum class DirTypes : int {
        EMPTY       = 0,        // If no option is set
        LORA        = 1 << 0,   // The LORA option represents the 0th bit
        CHECKPOINT  = 1 << 1,   // The CHECKPOINT option represents the 1st bit
        VAE         = 1 << 2,   // The VAE option represents the 2nd bit
        PRESETS     = 1 << 3,   // The PRESETS option represents the 3rd bit
        PROMPTS     = 1 << 4,   // The PROMPTS option represents the 4th bit
        NEG_PROMPTS = 1 << 5,   // The NEG_PROMPTS option represents the 5th bit
        TAESD       = 1 << 6,   // The TAESD option represents the 6th bit
        ESRGAN      = 1 << 7,   // The ESRGAN option represents the 7th bit
        CONTROLNET  = 1 << 8,   // The CONTROLNET option represents the 8th bit
        UPSCALER    = 1 << 9,   // The UPSCALER option represents the 9th bit
        EMBEDDING   = 1 << 10,  // The EMBEDDING option represents the 10th bit
        ALL         = -1,       // All options are set
        UNKNOWN     = -2,       // The unknown option
    };

    template <>
    struct enable_bitmask_operators<sd_gui_utils::DirTypes> {
        static constexpr bool enable = true;
    };

    inline bool filterByModelType(const sd_gui_utils::DirTypes modelType,
                                  sd_gui_utils::DirTypes filterType) {
        return static_cast<bool>(modelType & filterType);
    }

    inline std::unordered_map<DirTypes, std::string> dirtypes_str = {
        {sd_gui_utils::DirTypes::LORA, "LORA"},
        {sd_gui_utils::DirTypes::CHECKPOINT, "CHECKPOINT"},
        {sd_gui_utils::DirTypes::VAE, "VAE"},
        {sd_gui_utils::DirTypes::PRESETS, "PRESETS"},
        {sd_gui_utils::DirTypes::PROMPTS, "PROMPTS"},
        {sd_gui_utils::DirTypes::NEG_PROMPTS, "NEG_PROMPTS"},
        {sd_gui_utils::DirTypes::TAESD, "TAESD"},
        {sd_gui_utils::DirTypes::ESRGAN, "ESRGAN"},
        {sd_gui_utils::DirTypes::CONTROLNET, "CONTROLNET"},
        {sd_gui_utils::DirTypes::UPSCALER, "UPSCALER"},
        {sd_gui_utils::DirTypes::EMBEDDING, "EMBEDDING"},
        {sd_gui_utils::DirTypes::ALL, "ALL"},
        {sd_gui_utils::DirTypes::UNKNOWN, "UNKNOWN"}};

    enum CivitAiState { OK,
                        NOT_FOUND,
                        ERR,
                        NOT_CHECKED };
    inline const char* civitai_state_str[] = {
        "Ok",
        "Not found",
        "Parse error",
        "",
    };
    struct ModelFileInfo {
        std::string name;
        std::string path;
        std::string url;
        std::string poster;
        std::string sha256;
        uintmax_t size;
        std::string size_f;
        std::string meta_file;
        size_t hash_progress_size         = 0;
        size_t hash_fullsize              = 0;
        sd_gui_utils::DirTypes model_type = sd_gui_utils::DirTypes::UNKNOWN;
        std::string civitaiPlainJson;
        CivitAi::ModelInfo CivitAiInfo;
        sd_gui_utils::CivitAiState state = sd_gui_utils::CivitAiState::NOT_CHECKED;
        std::vector<std::string> preview_images;

        ModelFileInfo() = default;
        // Copy konstruktor
        ModelFileInfo(const sd_gui_utils::ModelFileInfo& other)
            : name(other.name), path(other.path), url(other.url), poster(other.poster), sha256(other.sha256), size(other.size), size_f(other.size_f), meta_file(other.meta_file), hash_progress_size(other.hash_progress_size), hash_fullsize(other.hash_fullsize), model_type(other.model_type), civitaiPlainJson(other.civitaiPlainJson), CivitAiInfo(other.CivitAiInfo), state(other.state), preview_images(other.preview_images) {}
        // Copy assignment operator
        ModelFileInfo& operator=(const sd_gui_utils::ModelFileInfo& other) {
            if (this != &other) {
                name               = other.name;
                path               = other.path;
                url                = other.url;
                poster             = other.poster;
                sha256             = other.sha256;
                size               = other.size;
                size_f             = other.size_f;
                meta_file          = other.meta_file;
                hash_progress_size = other.hash_progress_size;
                hash_fullsize      = other.hash_fullsize;
                model_type         = other.model_type;
                civitaiPlainJson   = other.civitaiPlainJson;
                CivitAiInfo        = other.CivitAiInfo;
                state              = other.state;
                preview_images     = other.preview_images;
            }
            return *this;
        }
        inline bool operator==(const sd_gui_utils::ModelFileInfo& rh) const {
            return path == rh.path;
        }
        inline bool operator==(const sd_gui_utils::ModelFileInfo rh) const {
            return path == rh.path;
        }
        inline bool operator==(const sd_gui_utils::ModelFileInfo* rh) const {
            return path == rh->path;
        }
        inline bool operator==(const std::string& otherPath) const {
            return path == otherPath;
        }
    };

    inline void to_json(nlohmann::json& j, const sd_gui_utils::ModelFileInfo& p) {
        j = nlohmann::json{{"name", wxString(p.name.c_str()).utf8_string()},
                           {"path", wxString(p.path.c_str()).utf8_string()},
                           {"url", p.url},
                           {"poster", wxString(p.poster.c_str()).utf8_string()},
                           {"sha256", p.sha256},
                           {"size", p.size},
                           {"size_f", p.size_f},
                           {"meta_file", wxString(p.meta_file.c_str()).utf8_string()},
                           {"model_type", (int)p.model_type},
                           {"civitaiPlainJson", p.civitaiPlainJson},
                           {"CivitAiInfo", p.CivitAiInfo},
                           {"state", (int)p.state},
                           {"preview_images", p.preview_images}

        };
    }

    inline void from_json(const nlohmann::json& j, ModelFileInfo& p) {
        if (j.contains("name")) {
            p.name = wxString::FromUTF8(j.at("name").get<std::string>());
        }

        if (j.contains("path")) {
            p.path = wxString::FromUTF8(j.at("path").get<std::string>());
        }

        if (j.contains("url")) {
            j.at("url").get_to(p.url);
        }

        if (j.contains("poster")) {
            p.poster = wxString::FromUTF8(j.at("poster").get<std::string>());
        }

        if (j.contains("sha256")) {
            j.at("sha256").get_to(p.sha256);
        }

        if (j.contains("size")) {
            j.at("size").get_to(p.size);
        }

        if (j.contains("size_f")) {
            j.at("size_f").get_to(p.size_f);
        }

        if (j.contains("meta_file")) {
            p.meta_file = wxString::FromUTF8(j.at("meta_file").get<std::string>().c_str());
        }

        if (j.contains("model_type")) {
            p.model_type = j.at("model_type").get<sd_gui_utils::DirTypes>();
        }

        if (j.contains("civitaiPlainJson")) {
            p.civitaiPlainJson = j.at("civitaiPlainJson").get<std::string>();
        }

        if (j.contains("CivitAiInfo")) {
            p.CivitAiInfo = j.at("CivitAiInfo").get<CivitAi::ModelInfo>();
        }
        if (j.contains("state")) {
            p.state = j.at("state").get<sd_gui_utils::CivitAiState>();
        }
        if (j.contains("preview_images")) {
            p.preview_images = j.at("preview_images").get<std::vector<std::string>>();
        }
    }
    inline std::string to_hex(std::array<uint8_t, 32> data) {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (uint8_t val : data) {
            oss << std::setw(2) << (unsigned int)val;
        }
        return oss.str();
    }

    inline unsigned long long Bytes2Megabytes(size_t bytes) {
        return bytes / (1024 * 1024);  // 1 MB = 1024 * 1024 bájt
    }

    struct cout_redirect {
        cout_redirect(std::streambuf* new_buffer)
            : old(std::cout.rdbuf(new_buffer)) {}

        ~cout_redirect() { std::cout.rdbuf(old); }

    private:
        std::streambuf* old;
    };
    enum imageTypes { JPG,
                      PNG,
                      WEBP };
    inline const char* image_types_str[] = {"JPG", "PNG", "WEBP"};
    struct config {
        std::string model                  = "";
        std::string vae                    = "";
        std::string lora                   = "";
        std::string embedding              = "";
        std::string taesd                  = "";
        std::string esrgan                 = "";
        std::string presets                = "";
        std::string output                 = "";
        std::string jobs                   = "";
        std::string controlnet             = "";
        std::string datapath               = "";
        std::string tmppath                = "";
        std::string thumbs_path            = "";
        std::string language               = "en";
        std::string output_filename_format = "[mode]_[jobid]_[seed]_[width]x[height]";
        bool keep_model_in_memory          = true;
        bool save_all_image                = true;
        int n_threads                      = 2;
        imageTypes image_type              = imageTypes::JPG;
        unsigned int image_quality         = 90;
        unsigned int png_compression_level = 0;
        bool show_notifications            = true;
        int notification_timeout           = 60;
        bool enable_civitai                = true;
    };
    inline std::string formatUnixTimestampToDate(long timestamp) {
        std::time_t time  = static_cast<std::time_t>(timestamp);
        std::tm* timeinfo = std::localtime(&time);

        std::stringstream ss;
        ss << (timeinfo->tm_year + 1900) << "-" << std::setw(2) << std::setfill('0')
           << (timeinfo->tm_mon + 1) << "-" << std::setw(2) << std::setfill('0')
           << timeinfo->tm_mday << " " << std::setw(2) << std::setfill('0')
           << timeinfo->tm_hour << ":" << std::setw(2) << std::setfill('0')
           << timeinfo->tm_min;

        return ss.str();
    }

    struct generator_preset {
        double cfg;
        int seed;
        int clip_skip;
        int steps;
        int width, height;
        sample_method_t sampler;
        std::string name;
        std::string mode;  // modes_str
        int batch;
        std::string type;  // types_str
        std::string path;
    };

    inline void to_json(nlohmann::json& j, const generator_preset& p) {
        j = nlohmann::json{
            {"cfg", p.cfg},
            {"seed", p.seed},
            {"clip_skip", p.clip_skip},
            {"steps", p.steps},
            {"width", p.width},
            {"height", p.height},
            {"name", p.name},
            {"mode", p.mode},
            {"type", p.type},
            {"sampler", (int)p.sampler},
            {"batch", p.batch},
        };
    }

    inline void from_json(const nlohmann::json& j, generator_preset& p) {
        j.at("cfg").get_to(p.cfg);
        j.at("seed").get_to(p.seed);
        j.at("clip_skip").get_to(p.clip_skip);
        j.at("steps").get_to(p.steps);
        j.at("width").get_to(p.width);
        j.at("height").get_to(p.height);
        j.at("name").get_to(p.name);
        j.at("mode").get_to(p.mode);
        if (j.contains("type")) {
            j.at("type").get_to(p.type);
        }
        j.at("batch").get_to(p.batch);
        p.sampler = j.at("sampler").get<sample_method_t>();
    }

    inline int GetCurrentUnixTimestamp() {
        const auto p1 = std::chrono::system_clock::now();
        return static_cast<int>(
            std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch())
                .count());
    }
    inline bool replace(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }
    inline std::string normalizePath(const std::string& messyPath) {
        std::filesystem::path path(messyPath);
        std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(path);
        std::string npath                   = canonicalPath.make_preferred().string();

        char toReplace   = '\\';
        char replacement = '/';

        std::replace(npath.begin(), npath.end(), toReplace, replacement);
        return npath;
    }
    // sd c++
    inline const char* sample_method_str[] = {
        "euler_a",
        "euler",
        "heun",
        "dpm2",
        "dpm++2s_a",
        "dpm++2m",
        "dpm++2mv2",
        "ipndm",
        "ipndm_v",
        "lcm",
    };
    inline const char* schedule_str[] = {
        "default",
        "discrete",
        "karras",
        "exponential",
        "ays",
        "gits"};

    inline const std::unordered_map<sample_method_t, std::string> samplerSdWebuiNames = {
        {sample_method_t::N_SAMPLE_METHODS, "Automatic"},
        {sample_method_t::EULER, "Euler"},
        {sample_method_t::EULER_A, "Euler a"},
        {sample_method_t::DPM2, "DPM2"},
        {sample_method_t::DPMPP2M, "DPM++ 2M"},
        {sample_method_t::DPMPP2Mv2, "Automatic"},
        {sample_method_t::DPMPP2S_A, "DPM++ 2S a"},
        {sample_method_t::HEUN, "Heun"},
        {sample_method_t::LCM, "LCM"},
        {sample_method_t::IPNDM, "IPNDM"},
        {sample_method_t::IPNDM_V, "IPNDM_V"},
    };

    inline const std::unordered_map<schedule_t, std::string> schedulerSdWebuiNames = {
        {schedule_t::DEFAULT, "Automatic"},
        {schedule_t::N_SCHEDULES, "Automatic"},
        {schedule_t::DISCRETE, "Automatic"},  // there is not discrete in sdwebui forge
        {schedule_t::KARRAS, "Karras"},
        {schedule_t::EXPONENTIAL, "Exponential"},
        {schedule_t::AYS, "Align Your Steps 11"},  // maybe this better
        {schedule_t::GITS, "Align Your Steps GITS"},
    };

    // f32, f16, q4_0, q4_1, q5_0, q5_1, q8_0
    inline std::map<int, std::string> sd_type_gui_names = {
        {SD_TYPE_COUNT, "Count"},
        {SD_TYPE_F32, "F32"},
        {SD_TYPE_F16, "F16"},
        {SD_TYPE_Q4_0, "Q4_0"},
        {SD_TYPE_Q4_1, "Q4_1"},
        {SD_TYPE_Q5_0, "Q5_0"},
        {SD_TYPE_Q5_1, "Q5_1"},
        {SD_TYPE_Q8_0, "Q8_0"},
        {SD_TYPE_Q2_K, "Q2_K"},
        {SD_TYPE_Q3_K, "Q3_K"},
        {SD_TYPE_Q4_K, "Q4_K"}};

    inline std::map<int, std::string> sd_scheduler_gui_names = {
        {0, "Default"},
        {1, "Discrete"},
        {2, "Karras"},
        {3, "Exponential"},
        {4, "Ays"},
        {5, "Gits"}

    };

    /* JSONize SD Params*/
    enum ThreadEvents {
        QUEUE,
        HASHING_PROGRESS,
        MODEL_LOAD_DONE,
        GENERATION_DONE,
        STANDALONE_HASHING_PROGRESS,
        STANDALONE_HASHING_DONE,
        HASHING_DONE,
        GENERATION_PROGRESS,
        SD_MESSAGE,
        MESSAGE,
        MODEL_INFO_DOWNLOAD_START,
        MODEL_INFO_DOWNLOAD_FAILED,
        MODEL_INFO_DOWNLOAD_FINISHED,
        MODEL_INFO_DOWNLOAD_IMAGES_START,
        MODEL_INFO_DOWNLOAD_IMAGES_PROGRESS,
        MODEL_INFO_DOWNLOAD_IMAGES_DONE,
        MODEL_INFO_DOWNLOAD_IMAGE_FAILED,
    };
    // sd c++

    inline const unsigned int generateRandomInt(unsigned int min, unsigned int max) {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<unsigned int> dis(min, max);
        return dis(gen);
    };

    inline std::pair<double, std::string> humanReadableFileSize(double bytes) {
        static const char* sizes[] = {"B", "KB", "MB", "GB", "TB"};
        long unsigned int div      = 0;
        double size                = bytes;

        while (size >= 1024 && div < (sizeof(sizes) / sizeof(*sizes))) {
            size /= 1024;
            div++;
        }

        return std::make_pair(size, sizes[div]);
    }

    inline std::string removeWhitespace(std::string str) {
        std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
        str.erase(end_pos, str.end());
        return str;
    }
    inline std::string tolower(std::string data) {
        std::transform(data.begin(), data.end(), data.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return data;
    }
    inline std::unordered_map<wxString, wxString> parseExifPrompts(wxString text) {
        size_t pos = text.find("charset=Unicode");
        if (pos != wxString::npos) {
            text.erase(pos, wxStrlen("charset=Unicode"));
        }

        std::unordered_map<wxString, wxString> result;
        result["prompt"] = "";

        wxArrayString lines_in_reverse;
        wxStringInputStream inputStream(text);
        wxTextInputStream textStream(inputStream);
        wxString line;

        while (!inputStream.Eof()) {
            line = textStream.ReadLine();
            lines_in_reverse.Add(line);
        }

        std::reverse(lines_in_reverse.begin(), lines_in_reverse.end());

        if (lines_in_reverse.IsEmpty()) {
            return result;  // Ha nincs adat, térjünk vissza üres map-pal
        }

        wxArrayString seglist;
        wxString firstline = lines_in_reverse[0];
        wxStringTokenizer tokenizer(firstline, ",");

        while (tokenizer.HasMoreTokens()) {
            wxString segment = tokenizer.GetNextToken();
            size_t pos       = segment.find(':');
            if (pos != wxString::npos) {
                wxString key = segment.SubString(0, pos - 1).Trim().MakeLower();
                wxString val = segment.SubString(pos + 1, segment.size() - 1).Trim();
                result[key]  = val;
            }
        }

        lines_in_reverse.RemoveAt(0);

        bool negativePromptFound = false;
        int counter              = 0;
        wxArrayString negativePromptLines;
        wxString negativePromptMarker = "Negative prompt: ";

        for (size_t i = 0; i < lines_in_reverse.size(); ++i) {
            wxString line = lines_in_reverse[i];
            if (line.StartsWith(negativePromptMarker)) {
                negativePromptLines.Add(line.Mid(negativePromptMarker.size()));
                negativePromptFound = true;
                counter++;
                break;
            } else {
                negativePromptLines.Add(line);
                counter++;
            }
        }

        for (int i = 0; i < counter; ++i) {
            lines_in_reverse.RemoveAt(0);
        }

        wxString prompt;

        for (size_t i = 0; i < negativePromptLines.size(); ++i) {
            prompt += negativePromptLines[negativePromptLines.size() - i - 1];
        }

        if (negativePromptFound) {
            result["negative_prompt"] = prompt;
            for (size_t i = 0; i < lines_in_reverse.size(); ++i) {
                result["prompt"] += lines_in_reverse[i];
            }
        } else {
            result["prompt"] = prompt;
        }

        return result;
    }

    inline static wxString AppendSuffixToFileName(const wxString& filePath, const wxString& suffix) {
        // Ellenőrizzük, hogy a fájl létezik-e
        if (!wxFileExists(filePath)) {
            std::cerr << "File does not exist: " << filePath << std::endl;
            return "";
        }

        wxFileName fileName(filePath);

        wxString baseName = fileName.GetName();
        fileName.SetName(baseName + "_" + suffix);

        return fileName.GetFullPath();
    }

    inline static wxString CreateFilePath(const wxString& filename, const wxString& extension, const wxString& folderPath, const wxString& suffix = "") {
        // Normalize path separator for the current OS
        wxString normalizedFilename = folderPath + wxFileName::GetPathSeparator() + filename + extension;
        normalizedFilename.Replace("\\", wxFileName::GetPathSeparator());
        normalizedFilename.Replace("/", wxFileName::GetPathSeparator());

        // Combine folder path and filename
        // wxFileName fullPath(folderPath, normalizedFilename, extension);
        wxFileName fullPath(normalizedFilename);

        // Ensure the parent directory exists
        wxString parentDir = fullPath.GetPath();

        // Check if the file exists and generate a unique filename
        wxString baseName = fullPath.GetName();  // Extract base name (e.g., "mi")
        wxString ext      = fullPath.GetExt();   // Extract extension (e.g., "txt")
        wxString path     = fullPath.GetPath();  // Extract path (e.g., "/home/valaki/kiement/vala")
        int counter       = 1;

        if (!wxDirExists(path)) {
            // create directories recursively
            if (!wxFileName::Mkdir(path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
                std::cerr << "Failed to create directory: " << path << std::endl;
            }
        }

        while (wxFileExists(fullPath.GetFullPath())) {
            // Generate a new unique name: baseName_counter_suffix.extension
            wxString newName = wxString::Format("%s_%d", baseName, counter);
            fullPath.Assign(path, newName, ext);
            ++counter;
        }
        if (!suffix.IsEmpty()) {
            return AppendSuffixToFileName(fullPath.GetFullPath(), suffix);
        }
        // Return the unique full file path as a string
        return fullPath.GetFullPath();
    }

    /*
     * Used at m_notebook1302OnNotebookPageChanged
     * FYI: keep the order as the gui builder
     */
    enum class GuiMainPanels : int {
        PANEL_QUEUE,
        PANEL_TEXT2IMG,
        PANEL_IMG2IMG,
        PANEL_UPSCALER,
        PANEL_IMAGEINFO,
        PANEL_MODELS
    };
}  // namespace sd_gui_utils
#endif
