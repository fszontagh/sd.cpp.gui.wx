## sd.cpp.gui.wx

Cross platform [wxWidgets](https://www.wxwidgets.org/) based [ Stable Diffusion C++](https://github.com/leejet/stable-diffusion.cpp) GuI



MIT License

### Features

- detect environment to which HW available for diffusion
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
- ROCm
- ..

### Compatibility

Please see [github releases](https://github.com/fszontagh/sd.cpp.gui.wx/releases) for installers. These packages are tested on Windows 11.

_Use only at your own risk!_
These releases just previews, so crashes may occur. For bugreport, please use [github Issuses](https://github.com/fszontagh/sd.cpp.gui.wx/issues). 


### Building on Linux / Windows

#### Requirements - Windows
- latest [vcpkg](https://vcpkg.io/en/) for dependency management
- latest [chalet](https://www.chalet-work.space/download) for building
- latest [cmake](https://cmake.org/)
- a compatible compiler

#### Requirements - Linux
- latest [chalet](https://www.chalet-work.space/download) for building
- latest [cmake](https://cmake.org/)
- build tools
- a compatible compiler
- wxWidgets 3.2 libs and header files

For configuration variations, please see the env-emaple_xxx, pick one and rename to .env

The open a terminal, and run (win / lin too):
``chalet c -c Release``
``chalet b all``
``chalet r sd.ui``