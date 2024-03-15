#ifndef __SD_GUI_CPP_CIVITAI_HELPER__
#define __SD_GUI_CPP_CIVITAI_HELPER__
#include <string>
#include <vector>
#ifndef JSON_DIAGNOSTICS
#define JSON_DIAGNOSTICS 1
#endif
#include "../libs/json.hpp"

// https://github.com/nlohmann/json/issues/1163#issuecomment-843988837
// ignore null and missing fields
#undef NLOHMANN_JSON_FROM
#define NLOHMANN_JSON_FROM(v1)                    \
    {                                             \
        auto iter = nlohmann_json_j.find(#v1);    \
        if (iter != nlohmann_json_j.end())        \
            if (!iter->is_null())                 \
                iter->get_to(nlohmann_json_t.v1); \
    }

namespace CivitAi
{
    struct model_stats
    {
        int downloadCount = -1;
        int ratingCount = -1;
        double rating = -1;

        model_stats() = default;
        model_stats(int downloadCount_, int ratingCount_, double rating_)
            : downloadCount(std::move(downloadCount_)),
              ratingCount(std::move(ratingCount_)),
              rating(std::move(rating_)){};

        model_stats &operator=(const model_stats &other)
        {
            if (this != &other)
            {
                downloadCount = other.downloadCount;
                ratingCount = other.ratingCount;
                rating = other.rating;
            }
            return *this;
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(model_stats, downloadCount, ratingCount, rating)

    struct Model
    {
        std::string name;
        std::string type;
        bool nsfw;
        bool poi;
        Model() = default;
        Model(std::string name_, std::string type_, bool nsfw_, bool poi_)
            : name(std::move(name_)),
              type(std::move(type_)),
              nsfw(std::move(nsfw_)),
              poi(std::move(poi_)){};
        // Copy assignment operator
        Model &operator=(const Model &other)
        {
            if (this != &other)
            {
                name = other.name;
                type = other.type;
                nsfw = other.nsfw;
                poi = other.poi;
            }
            return *this;
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Model, name, type, nsfw, poi)

    struct file_metadata
    {
        std::string fp;
        std::string size;
        std::string format;
        file_metadata() = default;
        file_metadata(std::string fp_, std::string size_, std::string format_)
            : fp(std::move(fp_)),
              size(std::move(size_)),
              format(std::move(format_)){};
        // Copy assignment operator
        file_metadata &operator=(const file_metadata &other)
        {
            if (this != &other)
            {
                fp = other.fp;
                size = other.size;
                format = other.format;
            }
            return *this;
        }
    };

    inline void to_json(nlohmann::json &nlohmann_json_j, const file_metadata &nlohmann_json_t)
    {
        nlohmann_json_j["fp"] = nlohmann_json_t.fp;
        nlohmann_json_j["size"] = nlohmann_json_t.size;
        nlohmann_json_j["format"] = nlohmann_json_t.format;
    }
    inline void from_json(const nlohmann::json &nlohmann_json_j, file_metadata &nlohmann_json_t)
    {
        const file_metadata nlohmann_json_default_obj{};
        if (nlohmann_json_j.contains("fp") && !nlohmann_json_j["fp"].is_null())
        {
            nlohmann_json_t.fp = nlohmann_json_j.value("fp", nlohmann_json_default_obj.fp);
        }
        if (nlohmann_json_j.contains("size") && !nlohmann_json_j["size"].is_null())
        {
            nlohmann_json_t.size = nlohmann_json_j.value("size", nlohmann_json_default_obj.size);
        }
        if (nlohmann_json_j.contains("format") && !nlohmann_json_j["format"].is_null())
        {
            nlohmann_json_t.format = nlohmann_json_j.value("format", nlohmann_json_default_obj.format);
        }
    }

    struct file_hashes
    {
        std::string AutoV1;
        std::string AutoV2;
        std::string SHA256;
        std::string CRC32;
        std::string BLAKE3;
        file_hashes() = default;
        file_hashes(std::string AutoV1_, std::string AutoV2_, std::string SHA256_, std::string CRC32_, std::string BLAKE3_)
            : AutoV1(std::move(AutoV1_)),
              AutoV2(std::move(AutoV2_)),
              SHA256(std::move(SHA256_)),
              CRC32(std::move(CRC32_)),
              BLAKE3(std::move(BLAKE3_)){};
        // Copy assignment operator
        file_hashes &operator=(const file_hashes &other)
        {
            if (this != &other)
            {
                AutoV1 = other.AutoV1;
                AutoV2 = other.AutoV2;
                SHA256 = other.SHA256;
                CRC32 = other.CRC32;
                BLAKE3 = other.BLAKE3;
            }
            return *this;
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(file_hashes, AutoV1, AutoV2, SHA256, CRC32, BLAKE3)

    struct file
    {
        int id = -1;
        double sizeKB = -1;
        std::string name;
        std::string type;
        CivitAi::file_metadata metadata;
        std::string pickleScanResult;
        CivitAi::file_hashes hashes;
        bool primary;
        std::string downloadUrl;

        file() = default;
        file(int id_,
             double sizeKB_,
             std::string name_,
             std::string type_,
             CivitAi::file_metadata metadata_,
             std::string pickleScanResult_,
             CivitAi::file_hashes hashes_,
             bool primary_,
             std::string downloadUrl_)
            : sizeKB(std::move(sizeKB_)),
              name(std::move(name_)),
              type(std::move(type_)),
              metadata(std::move(metadata_)),
              pickleScanResult(std::move(pickleScanResult_)),
              hashes(std::move(hashes_)),
              primary(std::move(primary)),
              downloadUrl(std::move(downloadUrl_)){};
        // Copy assignment operator
        file &operator=(const file &other)
        {
            if (this != &other)
            {
                id = other.id;
                sizeKB = other.sizeKB;
                name = other.name;
                type = other.type;
                metadata = other.metadata;
                pickleScanResult = other.pickleScanResult;
                hashes = other.hashes;
                primary = other.primary;
                downloadUrl = other.downloadUrl;
            }
            return *this;
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(file, id, sizeKB, name, type, metadata, pickleScanResult, hashes, primary, downloadUrl)

    struct image_metadata
    {
        std::string hash;
        size_t size;
        int width = -1;
        int height = -1;
        image_metadata() = default;
        image_metadata(std::string hash_, size_t size_, int width_, int height_)
            : hash(std::move(hash_)),
              size(std::move(size_)),
              width(std::move(width_)),
              height(std::move(height_)){};

        image_metadata &operator=(const image_metadata &other)
        {
            if (this != &other)
            {
                hash = other.hash;
                size = other.size;
                width = other.width;
                height = other.height;
            }
            return *this;
        }
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(image_metadata, hash, size, width, height)

    struct image_meta_hashes
    {
        std::string model;
        image_meta_hashes() = default;
        image_meta_hashes(std::string model_) : model(std::move(model_)){};

        image_meta_hashes &operator=(const image_meta_hashes &other)
        {
            if (this != &other)
            {
                model = other.model;
            }
            return *this;
        }
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(image_meta_hashes, model)

    struct image_meta
    {
        std::string Size;
        uint64_t seed = 0;
        std::string Model;
        int steps = -1;
        CivitAi::image_meta_hashes hashes;
        std::string prompt;
        std::string Version;
        std::string sampler;
        double cfgScale = -1;
        std::string negativePrompt;
        image_meta() = default;
        image_meta(std::string Size_,
                   uint64_t seed_,
                   std::string Model_,
                   int steps_,
                   CivitAi::image_meta_hashes hashes_,
                   std::string prompt_,
                   std::string Version_,
                   std::string sampler_,
                   double cfgScale_,
                   std::string negativePrompt_)
            : Size(std::move(Size_)),
              seed(seed_),
              Model(std::move(Model_)),
              steps(std::move(steps_)),
              hashes(std::move(hashes_)),
              prompt(std::move(prompt_)),
              Version(std::move(Version_)),
              sampler(std::move(sampler_)),
              cfgScale(std::move(cfgScale_)),
              negativePrompt(std::move(negativePrompt_)){};
        // Copy assignment operator
        image_meta &operator=(const image_meta &other)
        {
            if (this != &other)
            {
                Size = other.Size;
                seed = other.seed;
                Model = other.Model;
                steps = other.steps;
                hashes = other.hashes;
                prompt = other.prompt;
                Version = other.Version;
                sampler = other.sampler;
                cfgScale = other.cfgScale;
                negativePrompt = other.negativePrompt;
            }
            return *this;
        }
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(image_meta, Size, seed, Model, steps, hashes, prompt, Version, sampler, cfgScale, negativePrompt)

    struct image
    {
        std::string url;
        std::string nsfw;
        int width = 0;
        int height = 0;
        std::string hash;
        std::string type;
        std::string local_path;
        CivitAi::image_metadata metadata;
        std::string availability;
        CivitAi::image_meta meta;
        image() = default;
        image(std::string url_,
              std::string nsfw_,
              int width_,
              int height_,
              std::string hash_,
              std::string type_,
              std::string local_path_,
              CivitAi::image_metadata metadata_,
              std::string availability_,
              CivitAi::image_meta meta_)
            : url(std::move(url_)),
              nsfw(std::move(nsfw_)),
              width(std::move(width_)),
              height(std::move(height_)),
              hash(std::move(hash_)),
              type(std::move(type_)),
              local_path(std::move(local_path_)),
              metadata(std::move(metadata_)),
              availability(std::move(availability_)),
              meta(std::move(meta_)){};

        // Copy assignment operator
        image &operator=(const image &other)
        {
            if (this != &other)
            {
                url = other.url;
                nsfw = other.nsfw;
                width = other.width;
                height = other.height;
                hash = other.hash;
                type = other.type;
                local_path = other.local_path;
                metadata = other.metadata;
                availability = other.availability;
                meta = other.meta;
            }
            return *this;
        }
    };
    inline void to_json(nlohmann::json &nlohmann_json_j, const image &nlohmann_json_t)
    {
        nlohmann_json_j["url"] = nlohmann_json_t.url;
        nlohmann_json_j["nsfw"] = nlohmann_json_t.nsfw;
        nlohmann_json_j["width"] = nlohmann_json_t.width;
        nlohmann_json_j["height"] = nlohmann_json_t.height;
        nlohmann_json_j["hash"] = nlohmann_json_t.hash;
        nlohmann_json_j["type"] = nlohmann_json_t.type;
        nlohmann_json_j["local_path"] = nlohmann_json_t.local_path;
        nlohmann_json_j["metadata"] = nlohmann_json_t.metadata;
        nlohmann_json_j["availability"] = nlohmann_json_t.availability;
        nlohmann_json_j["meta"] = nlohmann_json_t.meta;
    }
    inline void from_json(const nlohmann::json &nlohmann_json_j, image &nlohmann_json_t)
    {
        const image nlohmann_json_default_obj{};
        nlohmann_json_t.url = nlohmann_json_j.value("url", nlohmann_json_default_obj.url);
        nlohmann_json_t.nsfw = nlohmann_json_j.value("nsfw", nlohmann_json_default_obj.nsfw);
        nlohmann_json_t.width = nlohmann_json_j.value("width", nlohmann_json_default_obj.width);
        nlohmann_json_t.height = nlohmann_json_j.value("height", nlohmann_json_default_obj.height);
        nlohmann_json_t.hash = nlohmann_json_j.value("hash", nlohmann_json_default_obj.hash);
        nlohmann_json_t.type = nlohmann_json_j.value("type", nlohmann_json_default_obj.type);

        if (nlohmann_json_j.contains("local_path") && !nlohmann_json_j["local_path"].is_null())
        {
            nlohmann_json_t.local_path = nlohmann_json_j.value("local_path", nlohmann_json_default_obj.type);
        }

        nlohmann_json_t.metadata = nlohmann_json_j.value("metadata", nlohmann_json_default_obj.metadata);
        nlohmann_json_t.availability = nlohmann_json_j.value("availability", nlohmann_json_default_obj.availability);
        if (!nlohmann_json_j["meta"].is_null())
        {
            nlohmann_json_t.meta = nlohmann_json_j.value("meta", nlohmann_json_default_obj.meta);
        }
        else
        {
            nlohmann_json_t.meta = nlohmann_json_default_obj.meta;
        }
    }

    struct ModelInfo
    {
        int id = -1;
        int modelId = -1;
        std::string name;
        std::string createdAt;
        std::string updatedAt;
        std::string status;
        std::string publishedAt;
        std::vector<std::string> trainedWords;
        std::string baseModel;
        std::string baseModelType;
        std::string description;
        CivitAi::model_stats stats;
        CivitAi::Model model;
        std::vector<CivitAi::file> files;
        std::vector<CivitAi::image> images;
        ModelInfo() = default;
        ModelInfo(
            int id_,
            int modelId_,
            std::string name_,
            std::string createdAt_,
            std::string updatedAt_,
            std::string status_,
            std::string publishedAt_,
            std::vector<std::string> trainedWords_,
            std::string baseModel_,
            std::string baseModelType_,
            std::string description_,
            CivitAi::model_stats stats_,
            CivitAi::Model model_,
            std::vector<CivitAi::file> files_,
            std::vector<CivitAi::image> images_)
            : id(std::move(id_)),
              modelId(std::move(modelId_)),
              name(std::move(name_)),
              createdAt(std::move(createdAt_)),
              updatedAt(std::move(updatedAt_)),
              status(std::move(status_)),
              publishedAt(std::move(publishedAt_)),
              trainedWords(std::move(trainedWords_)),
              baseModel(std::move(baseModel_)),
              baseModelType(std::move(baseModel_)),
              description(std::move(description_)),
              stats(std::move(stats_)),
              model(std::move(model_)),
              files(std::move(files_)),
              images(std::move(images_)){};
        // Copy assignment operator
        ModelInfo &operator=(const ModelInfo &other)
        {
            if (this != &other)
            {
                id = other.id;
                modelId = other.modelId;
                name = other.name;
                createdAt = other.createdAt;
                updatedAt = other.updatedAt;
                status = other.status;
                publishedAt = other.publishedAt;
                trainedWords = other.trainedWords;
                baseModel = other.baseModel;
                baseModelType = other.baseModelType;
                description = other.description;
                stats = other.stats;
                model = other.model;
                files = other.files;
                images = other.images;
            }
            return *this;
        }
    };
    inline void to_json(nlohmann::json &nlohmann_json_j, const ModelInfo &nlohmann_json_t)
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["modelId"] = nlohmann_json_t.modelId;
        nlohmann_json_j["name"] = nlohmann_json_t.name;
        nlohmann_json_j["createdAt"] = nlohmann_json_t.createdAt;
        nlohmann_json_j["updatedAt"] = nlohmann_json_t.updatedAt;
        nlohmann_json_j["status"] = nlohmann_json_t.status;
        nlohmann_json_j["publishedAt"] = nlohmann_json_t.publishedAt;
        nlohmann_json_j["trainedWords"] = nlohmann_json_t.trainedWords;
        nlohmann_json_j["baseModel"] = nlohmann_json_t.baseModel;
        nlohmann_json_j["baseModelType"] = nlohmann_json_t.baseModelType;
        nlohmann_json_j["description"] = nlohmann_json_t.description;
        nlohmann_json_j["stats"] = nlohmann_json_t.stats;
        nlohmann_json_j["model"] = nlohmann_json_t.model;
        nlohmann_json_j["files"] = nlohmann_json_t.files;
        nlohmann_json_j["images"] = nlohmann_json_t.images;
    }
    inline void from_json(const nlohmann::json &nlohmann_json_j, ModelInfo &nlohmann_json_t)
    {
        const ModelInfo nlohmann_json_default_obj{};

        if (nlohmann_json_j.contains("id") && !nlohmann_json_j["id"].is_null())
        {
            nlohmann_json_t.id = nlohmann_json_j.value("id", nlohmann_json_default_obj.id);
        }

        if (nlohmann_json_j.contains("modelId") && !nlohmann_json_j["modelId"].is_null())
        {
            nlohmann_json_t.modelId = nlohmann_json_j.value("modelId", nlohmann_json_default_obj.modelId);
        }

        if (nlohmann_json_j.contains("name") && !nlohmann_json_j["name"].is_null())
        {
            nlohmann_json_t.name = nlohmann_json_j.value("name", nlohmann_json_default_obj.name);
        }

        if (nlohmann_json_j.contains("createdAt") && !nlohmann_json_j["createdAt"].is_null())
        {
            nlohmann_json_t.createdAt = nlohmann_json_j.value("createdAt", nlohmann_json_default_obj.createdAt);
        }

        if (nlohmann_json_j.contains("updatedAt") && !nlohmann_json_j["updatedAt"].is_null())
        {
            nlohmann_json_t.updatedAt = nlohmann_json_j.value("updatedAt", nlohmann_json_default_obj.updatedAt);
        }

        if (nlohmann_json_j.contains("status") && !nlohmann_json_j["status"].is_null())
        {
            nlohmann_json_t.status = nlohmann_json_j.value("status", nlohmann_json_default_obj.status);
        }

        if (nlohmann_json_j.contains("publishedAt") && !nlohmann_json_j["publishedAt"].is_null())
        {
            nlohmann_json_t.publishedAt = nlohmann_json_j.value("publishedAt", nlohmann_json_default_obj.publishedAt);
        }

        if (nlohmann_json_j.contains("trainedWords") && !nlohmann_json_j["trainedWords"].is_null())
        {
            nlohmann_json_t.trainedWords = nlohmann_json_j.value("trainedWords", nlohmann_json_default_obj.trainedWords);
        }

        if (nlohmann_json_j.contains("baseModel") && !nlohmann_json_j["baseModel"].empty())
        {
            nlohmann_json_t.baseModel = nlohmann_json_j.value("baseModel", nlohmann_json_default_obj.baseModel);
        }

        if (nlohmann_json_j.contains("baseModelType") && !nlohmann_json_j["baseModelType"].is_null())
        {
            nlohmann_json_t.baseModelType = nlohmann_json_j.value("baseModelType", nlohmann_json_default_obj.baseModelType);
        }

        if (nlohmann_json_j.contains("description") && !nlohmann_json_j["description"].is_null())
        {
            nlohmann_json_t.description = nlohmann_json_j.value("description", nlohmann_json_default_obj.description);
        }

        if (nlohmann_json_j.contains("stats") && !nlohmann_json_j["stats"].is_null())
        {
            nlohmann_json_t.stats = nlohmann_json_j.value("stats", nlohmann_json_default_obj.stats);
        }

        if (nlohmann_json_j.contains("model") && !nlohmann_json_j["model"].is_null())
        {
            nlohmann_json_t.model = nlohmann_json_j.value("model", nlohmann_json_default_obj.model);
        }

        if (nlohmann_json_j.contains("files") && !nlohmann_json_j["files"].is_null())
        {
            nlohmann_json_t.files = nlohmann_json_j.value("files", nlohmann_json_default_obj.files);
        }

        if (nlohmann_json_j.contains("images") && !nlohmann_json_j["images"].is_null())
        {
            nlohmann_json_t.images = nlohmann_json_j.value("images", nlohmann_json_default_obj.images);
        }
    }
    // NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ModelInfo, name, creaedAt, updatedAt, status, publishedAt, baseModel, baseModelType, description, stats, files, images)
};

#endif