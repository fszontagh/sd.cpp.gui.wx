## Install dependencies - Ubuntu 24
```bash
sudo apt install -y
```

## Install dependencies - Ubuntu 22
```bash
sudo apt install -y
```

## Install dependencies - Windows

On windows for dependencies you need to install python to install Conan:
```Poweshell
python -m pip install --upgrade pip
pip install conan
```
Or you can download Conan installer from [here](https://conan.io/downloads)

To build, you need to install [cmake](https://cmake.org/download/) and [MSVC](https://visualstudio.microsoft.com/vs/).

## Prepare Conan profile & build collect dependencies

```Powershell
New-Item -ItemType Directory -Force -Path build
cd build
conan profile detect
conan install .. -s compiler.cppstd=17 --build=missing
```

For CUDA, VULKAN and HIPBLAS, you need to install the corresponding packages.
- CUDA: https://developer.nvidia.com/cuda-downloads
- VULKAN: https://vulkan.lunarg.com/sdk/home
- HIPBLAS: https://github.com/ROCmSoftwarePlatform/hipBLAS/releases

After collecting dependencies, you can build the project. All cmake options are the same, except for the following on windows:
- `--preset conan-default` - use conan preset
- `-DCUDA_TOOLKIT_ROOT_DIR=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.2` - set CUDA path. The path depends on your CUDA version.

For example, to build the project with CUDA only backend without the server component, run the following command:
```PowerShell
cmake -DCMAKE_BUILD_TYPE=Release -DSDGUI_SERVER=OFF -DSDGUI_CUDA=ON -DCUDA_TOOLKIT_ROOT_DIR=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.2 --preset conan-default ..
```


### Build only the GUI
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DSDGUI_SERVER=OFF -DSDGUI_CUDA=ON ..
```

### Build only backend libs
```bash
cmake .. -DSDGUI_SERVER=OFF -DSDGUI_WINDOWLESS=ON -DSDGUI_VULKAN=ON
```
### Build only backend libs without llama.cpp
```bash
cmake .. -DSDGUI_SERVER=OFF -DSDGUI_WINDOWLESS=ON -DSDGUI_VULKAN=ON -DSDGUI_LLAMA_DISABLE=ON
```

**Where the flags are:**
- `-DSDGUI_SERVER=OFF` - disable server
- `-DSDGUI_WINDOWLESS=ON` - disable window
- `-DSDGUI_VULKAN=ON` - enable vulkan

**Possible lib flags are:**
- `-SDGUI_AVX=ON` - enable AVX
- `-SDGUI_AVX2=ON` - enable AVX2
- `-SDGUI_AVX512=ON` - enable AVX512
- `-SDGUI_CUDA=ON` - enable CUDA
- `-SDGUI_VULKAN=ON` - enable Vulkan
- `-SDGUI_HIPBLAS=ON` - enable HIPBLAS

_When multiple flags are set, all lib variations are built._