#ifndef SRC_UI_IMAGEUTILS_H
#define SRC_UI_IMAGEUTILS_H
#include <png.h>
#include <cstddef>
#include <string>
#include "pngconf.h"
#include "ver.hpp"
namespace sd_gui_utils {
    /**
     * Resizes the given image to fit within the specified maximum width and
     * height while maintaining the image's aspect ratio.
     *
     * @param image The image to be resized.
     * @param maxWidth The maximum width of the resized image.
     * @param maxHeight The maximum height of the resized image.
     * @return A resized version of the given image.
     */
    inline wxImage ResizeImageToMaxSize(const wxImage& image, int maxWidth, int maxHeight) {
        int newWidth  = image.GetWidth();
        int newHeight = image.GetHeight();

        if (newWidth > maxWidth || newHeight > maxHeight) {
            double aspectRatio = static_cast<double>(newWidth) / static_cast<double>(newHeight);
            if (aspectRatio > 1.0) {
                newWidth  = maxWidth;
                newHeight = static_cast<int>(maxWidth / aspectRatio);
            } else {
                newHeight = maxHeight;
                newWidth  = static_cast<int>(maxHeight * aspectRatio);
            }
        }
        return image.Scale(newWidth, newHeight);
    };
    /**
     * Crops and resizes the given image to the specified target dimensions while
     * maintaining the image's aspect ratio. If the resized image does not fill the
     * target dimensions, it will be centered within a transparent background.
     *
     * @param originalImage The original image to be cropped and resized.
     * @param targetWidth The desired width of the resulting image.
     * @param targetHeight The desired height of the resulting image.
     * @return A wxImage that is resized to fit within the target dimensions, maintaining
     *         the aspect ratio, and centered with transparency if necessary.
     */
    inline wxImage cropResizeImage(const wxImage& originalImage, int targetWidth, int targetHeight) {
        int originalWidth  = originalImage.GetWidth();
        int originalHeight = originalImage.GetHeight();

        double aspectRatio = static_cast<double>(originalWidth) / static_cast<double>(originalHeight);
        int newWidth       = targetWidth;
        int newHeight      = targetHeight;

        if (originalWidth > targetWidth || originalHeight > targetHeight) {
            if (aspectRatio > 1.0) {
                newWidth  = targetWidth;
                newHeight = static_cast<int>(targetWidth / aspectRatio);
            } else {
                newHeight = targetHeight;
                newWidth  = static_cast<int>(targetHeight * aspectRatio);
            }
        }

        wxImage resizedImage = originalImage.Scale(newWidth, newHeight);

        if (newWidth < targetWidth || newHeight < targetHeight) {
            wxImage finalImage(targetWidth, targetHeight);
            finalImage.SetAlpha();

            finalImage.Paste(resizedImage, (targetWidth - newWidth) / 2,
                             (targetHeight - newHeight) / 2);
            return finalImage;
        }

        return resizedImage;
    }
    inline wxString cropResizeCacheName(int targetWidth, int targetHeight, const wxString& orig_filename, const wxString& cache_path) {
        auto cache_name_path     = std::filesystem::path(orig_filename.utf8_string());
        std::string ext          = cache_name_path.extension().string();
        auto filename            = cache_name_path.filename().replace_extension();
        std::string new_filename = filename.string() + "_" + std::to_string(targetWidth) + "x" + std::to_string(targetHeight);
        filename                 = filename.replace_filename(new_filename);
        filename                 = filename.replace_extension(ext);

        return wxString::FromUTF8Unchecked(sd_gui_utils::normalizePath(cache_path.utf8_string() + "/" + filename.string()));
    }

    inline wxImage cropResizeImage(const wxString image_path, int targetWidth, int targetHeight, const wxColour& backgroundColor, const wxString& cache_path = "") {
        wxString cache_name;
        if (!cache_path.empty()) {
            cache_name = sd_gui_utils::cropResizeCacheName(targetWidth, targetHeight, image_path, cache_path);
            if (std::filesystem::exists(cache_name.utf8_string())) {
                wxImage cached_img;
                cached_img.LoadFile(cache_name);
                return cached_img;
            }
        }
        auto originalImage = wxImage();

        if (!originalImage.LoadFile(image_path)) {
            return wxImage();
        }

        int originalWidth  = originalImage.GetWidth();
        int originalHeight = originalImage.GetHeight();

        double aspectRatio = static_cast<double>(originalWidth) / static_cast<double>(originalHeight);
        int newWidth       = targetWidth;
        int newHeight      = targetHeight;

        // Kiszámítjuk az új méreteket, hogy megtartsuk a képarányt
        if (originalWidth > targetWidth || originalHeight > targetHeight) {
            if (aspectRatio > 1.0) {
                // Szélesség alapján skálázzuk az új méretet
                newWidth  = targetWidth;
                newHeight = static_cast<int>(targetWidth / aspectRatio);
            } else {
                // Magasság alapján skálázzuk az új méretet
                newHeight = targetHeight;
                newWidth  = static_cast<int>(targetHeight * aspectRatio);
            }
        }

        // Méretezzük az eredeti képet az új méretekre
        wxImage resizedImage = originalImage.Scale(newWidth, newHeight);

        // Üres terület hozzáadása és háttérszínnel való töltése
        if (newWidth < targetWidth || newHeight < targetHeight) {
            wxImage finalImage(targetWidth, targetHeight);
            finalImage.SetRGB(wxRect(0, 0, targetWidth, targetHeight),
                              backgroundColor.Red(), backgroundColor.Green(),
                              backgroundColor.Blue());

            finalImage.Paste(resizedImage, (targetWidth - newWidth) / 2,
                             (targetHeight - newHeight) / 2);
            if (!cache_name.empty()) {
                finalImage.SaveFile(cache_name);
            }
            return finalImage;
        }
        if (!cache_name.empty()) {
            resizedImage.SaveFile(cache_name);
        }
        return resizedImage;
    };

    static std::unordered_map<wxString, wxString> ReadMetadata(const std::string& filepath) {
        std::unordered_map<wxString, wxString, std::hash<wxString>> metadata;

        size_t fileSize = std::filesystem::file_size(filepath) + 1024;

        FILE* fp = fopen(filepath.c_str(), "rb");
        if (!fp) {
            throw std::runtime_error("Failed to open file: " + filepath);
        }

        png_byte header[8];
        fread(header, 1, 8, fp);
        if (png_sig_cmp(header, 0, 8)) {
            fclose(fp);
            throw std::runtime_error("File is not a valid PNG: " + filepath);
        }

        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png) {
            fclose(fp);
            throw std::runtime_error("Failed to create PNG read struct");
        }

        png_infop info = png_create_info_struct(png);
        if (!info) {
            png_destroy_read_struct(&png, nullptr, nullptr);
            fclose(fp);
            throw std::runtime_error("Failed to create PNG info struct");
        }

        if (setjmp(png_jmpbuf(png))) {
            png_destroy_read_struct(&png, &info, nullptr);
            fclose(fp);
            throw std::runtime_error("Error during PNG processing");
        }

        png_set_compression_buffer_size(png, fileSize);

        png_uint_32 width;
        png_uint_32 height;
        int bit_depth;

        png_init_io(png, fp);
        png_set_sig_bytes(png, 8);  // We already read 8 bytes
        png_read_info(png, info);
        png_byte comp = png_get_compression_type(png, info);
        png_get_IHDR(png, info, &width, &height, &bit_depth, nullptr, nullptr, nullptr, nullptr);

        if (BUILD_TYPE != "Release") {
            std::cout << "Width: " << width << ", Height: " << height << ", Bit depth: " << bit_depth << " comp: " << comp << std::endl;
        }


        png_textp text_ptr;
        int num_text;
        if (png_get_text(png, info, &text_ptr, &num_text) > 0) {
            for (int i = 0; i < num_text; ++i) {
                wxString key = wxString::FromUTF8(text_ptr[i].key);
                wxString value;

                if (text_ptr[i].compression == PNG_TEXT_COMPRESSION_NONE ||
                    text_ptr[i].compression == PNG_TEXT_COMPRESSION_zTXt ||
                    text_ptr[i].compression == PNG_ITXT_COMPRESSION_NONE ||
                    text_ptr[i].compression == PNG_ITXT_COMPRESSION_zTXt) {
                    // num_text is 1, the key is not empty but the text is empty
                    if (text_ptr[i].text_length == 0) {
                        continue;
                    }
                    value = wxString::FromUTF8(text_ptr[i].text);
                } else {
                    throw std::runtime_error("Unsupported compression type in metadata.");
                }

                metadata[key] = value;
            }
        }

        png_destroy_read_struct(&png, &info, nullptr);
        fclose(fp);

        return metadata;
    }

    // Metaadat írása
    static void WriteMetadata(const std::string& filepath, const std::unordered_map<wxString, wxString>& newMeta, bool removeOthers = false) {
            std::unordered_map<wxString, wxString> existingMeta;
        if (!removeOthers) {
            existingMeta = ReadMetadata(filepath);

            for (const auto& pair : newMeta) {
                existingMeta[pair.first] = pair.second;
            }
        }else{
            existingMeta = newMeta;
        }

        FILE* input = fopen(filepath.c_str(), "rb");
        if (!input) {
            throw std::runtime_error("Failed to open file for reading: " + filepath);
        }

        png_structp readPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        png_infop readInfo  = png_create_info_struct(readPng);

        if (setjmp(png_jmpbuf(readPng))) {
            png_destroy_read_struct(&readPng, &readInfo, nullptr);
            fclose(input);
            throw std::runtime_error("Error during PNG reading");
        }

        png_init_io(readPng, input);
        png_read_info(readPng, readInfo);

        const png_uint_32 width   = png_get_image_width(readPng, readInfo);
        const png_uint_32 height  = png_get_image_height(readPng, readInfo);
        const png_byte color_type = png_get_color_type(readPng, readInfo);
        const png_byte bit_depth  = png_get_bit_depth(readPng, readInfo);

        png_read_update_info(readPng, readInfo);

        png_bytep* rowPointers = new png_bytep[height];
        for (png_uint_32 y = 0; y < height; ++y) {
            rowPointers[y] = static_cast<png_bytep>(malloc(png_get_rowbytes(readPng, readInfo)));
        }

        png_read_image(readPng, rowPointers);
        fclose(input);

        FILE* output = fopen(filepath.c_str(), "wb");
        if (!output) {
            png_destroy_read_struct(&readPng, &readInfo, nullptr);
            for (png_uint_32 y = 0; y < height; ++y) {
                free(rowPointers[y]);
            }
            throw std::runtime_error("Failed to open file for writing: " + filepath);
        }

        png_structp writePng = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        png_infop writeInfo  = png_create_info_struct(writePng);

        if (setjmp(png_jmpbuf(writePng))) {
            png_destroy_write_struct(&writePng, &writeInfo);
            fclose(output);
            for (png_uint_32 y = 0; y < height; ++y) {
                free(rowPointers[y]);
            }
            throw std::runtime_error("Error during PNG writing");
        }

        png_init_io(writePng, output);

        png_set_IHDR(writePng, writeInfo, width, height, bit_depth, color_type,
                     png_get_interlace_type(readPng, readInfo),
                     png_get_compression_type(readPng, readInfo),
                     png_get_filter_type(readPng, readInfo));

        png_write_info(writePng, writeInfo);

        if (!existingMeta.empty()) {
            std::vector<png_text> textChunks;
            for (const auto& pair : existingMeta) {
                png_text text;
                text.compression = PNG_TEXT_COMPRESSION_NONE;
                text.key         = const_cast<char*>(pair.first.ToUTF8().data());
                text.text        = const_cast<char*>(pair.second.ToUTF8().data());
                text.text_length = pair.second.ToUTF8().length();
                textChunks.push_back(text);
            }
            png_set_text(writePng, writeInfo, textChunks.data(), static_cast<int>(textChunks.size()));
            png_write_info(writePng, writeInfo);
        }

        png_write_image(writePng, rowPointers);
        png_write_end(writePng, nullptr);

        fclose(output);
        png_destroy_read_struct(&readPng, &readInfo, nullptr);
        png_destroy_write_struct(&writePng, &writeInfo);
        for (png_uint_32 y = 0; y < height; ++y) {
            free(rowPointers[y]);
        }
    }

}
#endif  // SRC_UI_IMAGEUTILS_H
