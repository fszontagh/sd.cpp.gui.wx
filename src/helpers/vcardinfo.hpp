#ifndef __DETECT_VCARD__
#define __DETECT_VCARD__

#ifndef WIN32
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

inline bool isAmdGPU()
{
    for (const auto &entry : fs::directory_iterator("/sys/class/drm"))
    {
        if (fs::is_directory(entry.path()))
        {
            std::string path = entry.path().string();
            std::ifstream file(path + "/device/vendor");
            if (file.is_open())
            {
                std::string vendor;
                std::getline(file, vendor);
                if (vendor.find("AMD") != std::string::npos)
                {
                    return true;
                }
                file.close();
            }
        }
    }
    return false;
}

inline bool isNvidiaGPU()
{
    // Check if NVIDIA driver directory exists
    if (!std::filesystem::exists("/proc/driver/nvidia")) {
        return false;
    }

    // Iterate over all directories in /proc/driver/nvidia/gpus/
    for (const auto& entry : fs::directory_iterator("/proc/driver/nvidia/gpus"))
    {
        if (fs::is_directory(entry))
        {
            std::string gpuInfoPath = entry.path().string() + "/information";
            std::ifstream file(gpuInfoPath);

            if (file.is_open())
            {
                std::string line;
                while (std::getline(file, line))
                {
                    if (line.find("NVIDIA") != std::string::npos)
                    {
                        return true;
                    }
                }
                file.close();
            }
        }
    }

    return false;
}

#else
#include <iostream>
#include <Windows.h>
#include <string>
#include <regex>
#include <wbemidl.h>
#include <comutil.h>

bool isNvidiaGPU()
{
    HKEY hKey;
    DWORD dataType, dataSize;
    char data[1024];
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\nvlddmkm", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        dataSize = sizeof(data);
        if (RegQueryValueExA(hKey, "ImagePath", 0, &dataType, (LPBYTE)data, &dataSize) == ERROR_SUCCESS)
        {
            std::string imagePath(data);
            if (imagePath.find("nvlddmkm.sys") != std::string::npos)
            {
                RegCloseKey(hKey);
                return true;
            }
        }
        RegCloseKey(hKey);
    }
    return false;
}

bool isAmdGPU()
{
    bool isAmd = false;
    IWbemLocator *pLoc = nullptr;
    IWbemServices *pSvc = nullptr;

    CoInitialize(NULL);
    HRESULT hr = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL);

    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
        if (SUCCEEDED(hr))
        {
            hr = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
            if (SUCCEEDED(hr))
            {
                hr = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
                if (SUCCEEDED(hr))
                {
                    IEnumWbemClassObject *pEnumerator = NULL;
                    hr = pSvc->ExecQuery(
                        bstr_t("WQL"),
                        bstr_t("SELECT * FROM Win32_PnPEntity"),
                        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                        NULL,
                        &pEnumerator);

                    if (SUCCEEDED(hr))
                    {
                        IWbemClassObject *pclsObj = NULL;
                        ULONG uReturn = 0;

                        while (pEnumerator)
                        {
                            HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

                            if (0 == uReturn)
                            {
                                break;
                            }

                            VARIANT vtProp;

                            hr = pclsObj->Get(L"Description", 0, &vtProp, 0, 0);

                            if (SUCCEEDED(hr))
                            {
                                if (vtProp.vt == VT_BSTR && vtProp.bstrVal != nullptr)
                                {
                                    std::wstring description(vtProp.bstrVal);
                                    if (description.find(L"AMD") != std::wstring::npos || description.find(L"Advanced Micro Devices") != std::wstring::npos)
                                    {
                                        isAmd = true;
                                        VariantClear(&vtProp);
                                        break;
                                    }
                                }
                            }

                            VariantClear(&vtProp);
                            pclsObj->Release();
                        }
                        pEnumerator->Release();
                    }
                }
            }
        }
    }

    if (pLoc)
        pLoc->Release();
    if (pSvc)
        pSvc->Release();
    CoUninitialize();

    return isAmd;
}

#endif

#endif
