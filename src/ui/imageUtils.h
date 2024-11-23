#ifndef SRC_UI_IMAGEUTILS_H
#define SRC_UI_IMAGEUTILS_H
namespace sd_gui_utils {
    inline wxImage ResizeImageToMaxSize(const wxImage& image, int maxWidth, int maxHeight) {
        int newWidth  = image.GetWidth();
        int newHeight = image.GetHeight();

        // Az új méreteket úgy határozzuk meg, hogy megtartsuk a képarányt
        if (newWidth > maxWidth || newHeight > maxHeight) {
            double aspectRatio =
                static_cast<double>(newWidth) / static_cast<double>(newHeight);
            if (aspectRatio > 1.0) {
                // Szélesség korlátozó dimenzió, a magasságot arányosan beállítjuk
                newWidth  = maxWidth;
                newHeight = static_cast<int>(maxWidth / aspectRatio);
            } else {
                // Magasság korlátozó dimenzió, a szélességet arányosan beállítjuk
                newHeight = maxHeight;
                newWidth  = static_cast<int>(maxHeight * aspectRatio);
            }
        }

        // Méretezés az új méretekre
        return image.Scale(newWidth, newHeight);
    };
    inline wxImage cropResizeImage(const wxImage& originalImage, int targetWidth, int targetHeight) {
        int originalWidth  = originalImage.GetWidth();
        int originalHeight = originalImage.GetHeight();

        double aspectRatio =
            static_cast<double>(originalWidth) / static_cast<double>(originalHeight);
        int newWidth  = targetWidth;
        int newHeight = targetHeight;

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

        // Üres terület hozzáadása és transzparens töltése
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
    }
}
#endif  // SRC_UI_IMAGEUTILS_H
