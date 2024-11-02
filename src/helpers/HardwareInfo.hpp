#ifndef HARDWAREINFO_HPP
#define HARDWAREINFO_HPP
#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class HardwareInfo {
public:
    std::vector<std::string> getAvailableDevices() {
        std::vector<std::string> devices;

        // CPU hozzáadása az első elemként
        std::string cpuName = getCPUName();
        if (!cpuName.empty()) {
            devices.push_back(cpuName);
        } else {
            devices.push_back("Unknown CPU");
        }

        // GPU hozzáadása
        auto gpus = getGPUList();
        devices.insert(devices.end(), gpus.begin(), gpus.end());

        return devices;
    }

private:
    // CPU név lekérése
    std::string getCPUName() {
#ifdef _WIN32
        return execCommand("wmic cpu get name");
#elif __linux__
        return execCommand("cat /proc/cpuinfo | grep 'model name' | head -1 | cut -d ':' -f2 | sed 's/^ *//g'");
#else
        return "Unsupported OS";
#endif
    }

    // GPU lista lekérése
    std::vector<std::string> getGPUList() {
        std::vector<std::string> gpus;
#ifdef _WIN32
        std::string output = execCommand("wmic path win32_videocontroller get name");
        parseWindowsGPUList(output, gpus);
#elif __linux__
        std::string output = execCommand("lspci | grep -i 'vga\\|3d\\|display'");
        parseLinuxGPUList(output, gpus);
#else
        gpus.push_back("Unsupported OS");
#endif
        return gpus;
    }

    // Parancs végrehajtása és kimenet visszaadása
    std::string execCommand(const char* cmd) {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe)
            throw std::runtime_error("Failed to run command");
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }

    // Windows GPU lista feldolgozása
    void parseWindowsGPUList(const std::string& output, std::vector<std::string>& gpus) {
        std::istringstream iss(output);
        std::string line;
        bool firstLine = true;
        while (std::getline(iss, line)) {
            if (firstLine) {  // Az első sor átugrása
                firstLine = false;
                continue;
            }
            if (!line.empty())
                gpus.push_back(line);
        }
    }

    // Linux GPU lista feldolgozása
    void parseLinuxGPUList(const std::string& output, std::vector<std::string>& gpus) {
        std::istringstream iss(output);
        std::string line;
        while (std::getline(iss, line)) {
            gpus.push_back(line);
        }
    }
};

#endif  // HARDWAREINFO_HPP