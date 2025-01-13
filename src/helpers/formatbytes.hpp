#ifndef _SDGUI_HELPERS_FORMATBYTES_H
#define _SDGUI_HELPERS_FORMATBYTES_H
namespace sd_gui_utils {
    inline std::pair<double, std::string> formatbytes(double bytes) {
        static const char* sizes[] = {"B", "KB", "MB", "GB", "TB"};
        long unsigned int div      = 0;
        double size                = bytes;

        while (size >= 1024 && div < (sizeof(sizes) / sizeof(*sizes))) {
            size /= 1024;
            div++;
        }

        return std::make_pair(size, sizes[div]);
    }
    inline std::pair<double, std::string> formatbytes(size_t bytes) {
        return formatbytes(static_cast<double>(bytes));
    }
}
#endif  // _SDGUI_HELPERS_FORMATBYTES_H