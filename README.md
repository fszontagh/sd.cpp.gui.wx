## sd.cpp.gui.wx

Cross platform [wxWidgets](https://www.wxwidgets.org/) based [ Stable Diffusion C++](https://github.com/leejet/stable-diffusion.cpp) GuI



MIT License

### Features

- text2img generation
- controlnet support
- image2image generation
- upscaler with some limitations
- minimalistic model management, model info from CivitAi.com
- vae support
- presets management (save / restore some config from UI)
- generation queue & queue list
- progress bar in the queuelist, speed (it/s or s/it)
- restore last item from queue list, auto start last pending job
- clone queue item, copy generation data from queue item
- save generation metadata into images
- load generation data from images
- image drag & drop

### External libraries
- [stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp) by leejet
- [wxWidgets](https://www.wxwidgets.org/) Cross-Platform GUI Library
- [Exiv2](https://github.com/Exiv2/exiv2)
- [curlpp](https://www.curlpp.org/) C++ wrapper around libcURL
- [nlohmann/json](https://github.com/nlohmann/json) JSON for Modern C++
- [{fmt}](https://fmt.dev/)
- CUDA 

### Compatibility

Please see [github releases](https://github.com/fszontagh/sd.cpp.gui.wx/releases) for installers. These packages are tested on Windows 11. On linux the GUI may fall apart. 

_Use only at your own risk!_
These releases just previews, so crashes may occur. For bugreport, please use [github Issuses](https://github.com/fszontagh/sd.cpp.gui.wx/issues). 


### Building on Linux / Windows

#### Requirements
- vcpkg for dependency management
- latest cmake
- a compatible compiler
- if you don't want to build CUDA version (only cpu with or without flags), please remove the "CUDA" from the _vcpkg.json_ file

Configure with **AVX**
 ```Bash
 cmake -B build/ -S . -DCMAKE_TOOLCHAIN_FILE=%vcpkg_root_dir%/scripts/buildsystems/vcpkg.cmake -DGGML_AVX512=OFF -DGGML_AVX2=OFF -DGGML_AVX=ON -DSD_CUBLAS=OFF
 ```

Configure with **AVX2**
 ```Bash
 cmake -B build/ -S . -DCMAKE_TOOLCHAIN_FILE=%vcpkg_root_dir%/scripts/buildsystems/vcpkg.cmake -DGGML_AVX512=OFF -DGGML_AVX2=ON -DGGML_AVX=OFF -DSD_CUBLAS=OFF
 ```

Configure with **AVX512**
 ```Bash
 cmake -B build/ -S . -DCMAKE_TOOLCHAIN_FILE=%vcpkg_root_dir%/scripts/buildsystems/vcpkg.cmake -DGGML_AVX512=ON -DGGML_AVX2=ON -DGGML_AVX=ON -DSD_CUBLAS=OFF
 ```

Configure with **CUDA**
 ```Bash
 cmake -B build/ -S . -DCMAKE_TOOLCHAIN_FILE=%vcpkg_root_dir%/scripts/buildsystems/vcpkg.cmake -DSD_CUBLAS=ON
 ```

Build
 ```Bash
 cmake --build build/ --config Release
 ```
VCPKG will deploy the dependencies.
