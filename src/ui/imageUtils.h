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

    inline void ResizeImageToMaxSize(std::shared_ptr<wxImage> image, int maxWidth, int maxHeight) {
        int newWidth  = image->GetWidth();
        int newHeight = image->GetHeight();

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
        wxImage tmpImage = image->Scale(newWidth, newHeight);
        *image           = tmpImage;
    };

    /**
     * Resize the given image to fit within the specified maximum width and
     * height while maintaining the image's aspect ratio. If the resized image
     * is smaller than the target width and height, it will be centered within a
     * background image of the target size.
     *
     * @param originalImage The image to be resized.
     * @param targetWidth The maximum width of the resized image.
     * @param targetHeight The maximum height of the resized image.
     * @return A resized version of the given image.
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

            finalImage.Paste(resizedImage, (targetWidth - newWidth) / 2, (targetHeight - newHeight) / 2);
            return finalImage;
        }

        return resizedImage;
    }
    inline wxFileName cropResizeCacheName(int targetWidth, int targetHeight, const wxString& orig_filename, const wxString& cache_path) {
        wxFileName fn(orig_filename);
        auto ext          = fn.GetExt();
        auto filename     = fn.GetName();
        auto new_filename = filename + "_" + wxString::Format("%dx%d", targetWidth, targetHeight);
        fn.SetName(new_filename);
        fn.SetExt(ext);
        fn.SetPath(cache_path);
        return fn;
    }

    inline wxImage cropResizeImage(const wxString image_path, int targetWidth, int targetHeight, const wxColour& backgroundColor = wxColour(51, 51, 51, wxALPHA_TRANSPARENT), const wxString& cache_path = "") {
        wxFileName cache_name;
        if (!cache_path.empty()) {
            cache_name = sd_gui_utils::cropResizeCacheName(targetWidth, targetHeight, image_path, cache_path);
            if (cache_name.Exists()) {
                wxImage cached_img;
                cached_img.LoadFile(cache_name.GetAbsolutePath());
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
            finalImage.SetRGB(wxRect(0, 0, targetWidth, targetHeight), backgroundColor.Red(), backgroundColor.Green(), backgroundColor.Blue());
            finalImage.Paste(resizedImage, (targetWidth - newWidth) / 2, (targetHeight - newHeight) / 2);
            finalImage.SaveFile(cache_name.GetAbsolutePath());
            return finalImage;
        }
        resizedImage.SaveFile(cache_name.GetAbsolutePath());
        return resizedImage;
    };
    struct wxEnlargeImageSizes {
        int top    = 0;
        int right  = 0;
        int bottom = 0;
        int left   = 0;
        void ResetSizes() {
            top    = 0;
            right  = 0;
            bottom = 0;
            left   = 0;
        }
    };

    inline void CropOrFillBitmap(std::shared_ptr<wxBitmap> bitmap, const wxEnlargeImageSizes& enlargeSizes, wxColour fillColor) {
        if (bitmap == nullptr || !bitmap->IsOk()) {
            return;
        }

        // Convert wxBitmap to wxImage for manipulation
        wxImage image = bitmap->ConvertToImage();

        int originalWidth  = image.GetWidth();
        int originalHeight = image.GetHeight();

        int newWidth  = originalWidth + enlargeSizes.left + enlargeSizes.right;
        int newHeight = originalHeight + enlargeSizes.top + enlargeSizes.bottom;

        wxImage newImage(newWidth, newHeight);

        // Fill the new image with the fill color
        newImage.SetRGB(wxRect(0, 0, newWidth, newHeight), fillColor.Red(), fillColor.Green(), fillColor.Blue());

        int xOffset = enlargeSizes.left + (newWidth - originalWidth - enlargeSizes.left - enlargeSizes.right) / 2;
        int yOffset = enlargeSizes.top + (newHeight - originalHeight - enlargeSizes.top - enlargeSizes.bottom) / 2;

        // Paste the original image onto the new image
        newImage.Paste(image, xOffset, yOffset);

        // Convert the modified wxImage back to wxBitmap
        *bitmap = wxBitmap(newImage);
    }
    inline void CropOrFillImage(std::shared_ptr<wxImage> image, const wxEnlargeImageSizes& enlargeSizes, wxColour fillColor) {
        if (image == nullptr) {
            return;
        }
        int originalWidth  = image->GetWidth();
        int originalHeight = image->GetHeight();

        int newWidth  = originalWidth + enlargeSizes.left + enlargeSizes.right;
        int newHeight = originalHeight + enlargeSizes.top + enlargeSizes.bottom;

        wxImage newImage(newWidth, newHeight);

        newImage.SetRGB(wxRect(0, 0, newWidth, newHeight), fillColor.Red(), fillColor.Green(), fillColor.Blue());

        int xOffset = enlargeSizes.left + (newWidth - originalWidth - enlargeSizes.left - enlargeSizes.right) / 2;
        int yOffset = enlargeSizes.top + (newHeight - originalHeight - enlargeSizes.top - enlargeSizes.bottom) / 2;

        newImage.Paste(*image, xOffset, yOffset);

        *image = newImage;
    }

    inline void CropOrFillImage(wxImage& image, int top, int right, int bottom, int left, wxColour fillColor) {
        int originalWidth  = image.GetWidth();
        int originalHeight = image.GetHeight();

        int newWidth  = originalWidth + left + right;
        int newHeight = originalHeight + top + bottom;

        wxImage newImage(newWidth, newHeight);

        newImage.SetRGB(wxRect(0, 0, newWidth, newHeight), fillColor.Red(), fillColor.Green(), fillColor.Blue());

        int xOffset = left + (newWidth - originalWidth - left - right) / 2;
        int yOffset = top + (newHeight - originalHeight - top - bottom) / 2;

        newImage.Paste(image, xOffset, yOffset);

        image = newImage;
    }
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
        } else {
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
    inline void ResizeWithBorder(wxBitmap& bitmap, int top, int right, int bottom, int left) {
        if (top < 0 || right < 0 || bottom < 0 || left < 0) {
            return;
        }

        int originalWidth  = bitmap.GetWidth();
        int originalHeight = bitmap.GetHeight();

        int newWidth  = originalWidth + left + right;
        int newHeight = originalHeight + top + bottom;

        wxBitmap newBitmap(newWidth, newHeight);
        wxMemoryDC dc(newBitmap);

        dc.SetBackground(*wxGREY_BRUSH);
        dc.Clear();

        int xOffset = left + (newWidth - originalWidth - left - right) / 2;
        int yOffset = top + (newHeight - originalHeight - top - bottom) / 2;

        dc.DrawBitmap(bitmap, xOffset, yOffset, false);

        dc.SelectObject(wxNullBitmap);
        bitmap = newBitmap;
    }

    /**
     * Inverts white and transparent pixels in the given bitmap.
     *
     * For each pixel in the bitmap:
     * - If the pixel is white (RGB = (255, 255, 255)) and has an alpha value of greater than 0, it will be made transparent (alpha = 0).
     * - If the pixel is not white and has an alpha value of less than 255, it will be made white (RGB = (255, 255, 255)) and fully opaque (alpha = 255).
     *
     * The image is then converted back to a bitmap and the scale factor is set to the original value.
     *
     * @param bitmap The bitmap to be modified.
     */
    inline void InvertWhiteAndTransparent(wxBitmap& bitmap) {
        auto oldScale = bitmap.GetScaleFactor();
        wxImage image = bitmap.ConvertToImage();
        if (!image.HasAlpha()) {
            image.InitAlpha();
        }

        for (int x = 0; x < image.GetWidth(); ++x) {
            for (int y = 0; y < image.GetHeight(); ++y) {
                unsigned char red   = image.GetRed(x, y);
                unsigned char green = image.GetGreen(x, y);
                unsigned char blue  = image.GetBlue(x, y);
                unsigned char alpha = image.GetAlpha(x, y);

                if (red == 255 && green == 255 && blue == 255 && alpha > 0) {
                    image.SetAlpha(x, y, 0);  // make it transparent
                }

                else if (alpha < 255) {
                    image.SetRGB(x, y, 255, 255, 255);  // make it white
                    image.SetAlpha(x, y, 255);
                }
            }
        }

        bitmap = wxBitmap(image);
        bitmap.SetScaleFactor(oldScale);
    }

    inline void InvertWhiteAndTransparent(std::shared_ptr<wxBitmap> bitmap) {
        if (!bitmap)
            return;

        auto oldScale = bitmap->GetScaleFactor();
        wxImage image = bitmap->ConvertToImage();

        if (!image.HasAlpha()) {
            image.InitAlpha();
        }

        for (int x = 0; x < image.GetWidth(); ++x) {
            for (int y = 0; y < image.GetHeight(); ++y) {
                unsigned char red   = image.GetRed(x, y);
                unsigned char green = image.GetGreen(x, y);
                unsigned char blue  = image.GetBlue(x, y);
                unsigned char alpha = image.GetAlpha(x, y);

                if (red == 255 && green == 255 && blue == 255 && alpha > 0) {
                    image.SetAlpha(x, y, 0);  // Make it transparent
                } else if (alpha < 255) {
                    image.SetRGB(x, y, 255, 255, 255);  // Make it white
                    image.SetAlpha(x, y, 255);
                }
            }
        }

        *bitmap = wxBitmap(image);
        bitmap->SetScaleFactor(oldScale);
    }

    inline void convertMaskImageToTransparent(wxImage& image) {
        if (!image.HasAlpha()) {
            image.InitAlpha();
        }
        for (int x = 0; x < image.GetWidth(); ++x) {
            for (int y = 0; y < image.GetHeight(); ++y) {
                // only convert black pixels to transparent
                auto red   = image.GetRed(x, y);
                auto green = image.GetGreen(x, y);
                auto blue  = image.GetBlue(x, y);

                // if not white
                if (red == 0 && green == 0 && blue == 0) {
                    image.SetRGB(x, y, 0, 0, 0);
                    image.SetAlpha(x, y, 0);
                }
            }
        }
    }

    inline void convertMaskImageTransparentToBlack(wxImage& image) {
        if (!image.HasAlpha()) {
            image.InitAlpha();
        }
        for (int x = 0; x < image.GetWidth(); ++x) {
            for (int y = 0; y < image.GetHeight(); ++y) {
                // only convert transparent pixels to black
                auto red   = image.GetRed(x, y);
                auto green = image.GetGreen(x, y);
                auto blue  = image.GetBlue(x, y);
                auto alpha = image.GetAlpha(x, y);
                if (alpha < 255) {
                    image.SetRGB(x, y, 0, 0, 0);
                    image.SetAlpha(x, y, 255);
                }
            }
        }
        if (image.HasAlpha()) {
            image.ClearAlpha();
        }
    }

    inline void blendImageOnBlackBackground(wxImage& image) {
        if (!image.HasAlpha()) {
            image.InitAlpha();
        }

        wxImage blackBackground(image.GetWidth(), image.GetHeight());
        blackBackground.SetRGB(wxRect(0, 0, image.GetWidth(), image.GetHeight()), 0, 0, 0);

        for (int x = 0; x < image.GetWidth(); ++x) {
            for (int y = 0; y < image.GetHeight(); ++y) {
                unsigned char srcRed   = image.GetRed(x, y);
                unsigned char srcGreen = image.GetGreen(x, y);
                unsigned char srcBlue  = image.GetBlue(x, y);
                unsigned char srcAlpha = image.GetAlpha(x, y);

                float alphaFactor = static_cast<float>(srcAlpha) / 255.0f;

                unsigned char blendedRed   = static_cast<unsigned char>(srcRed * alphaFactor);
                unsigned char blendedGreen = static_cast<unsigned char>(srcGreen * alphaFactor);
                unsigned char blendedBlue  = static_cast<unsigned char>(srcBlue * alphaFactor);

                blackBackground.SetRGB(x, y, blendedRed, blendedGreen, blendedBlue);
            }
        }

        image = blackBackground;
    }
    inline std::string GetAspectRatioString(const wxImage& image) {
        int width  = image.GetWidth();
        int height = image.GetHeight();

        if (height == 0) {
            return "Invalid";
        }

        int gcd = std::gcd(width, height);

        int aspectW = width / gcd;
        int aspectH = height / gcd;

        return std::to_string(aspectW) + ":" + std::to_string(aspectH);
    }

}
#endif  // SRC_UI_IMAGEUTILS_H
