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

- build tools

  ```
  apt install build-essential
  ```

- wxWidgets 3.2 libs and header files from [PPA](https://launchpad.net/~sjr/+archive/ubuntu/wx32)

  ```
  add-apt-repository ppa:sjr/wx32
  apt install wx3.2-headers wx3.2-i18n "libwxgtk*"
  ```

- exiv2

  ```
  apt install libexiv2-dev
  ```

- OpenSSL:

  ```
  apt install libssl-dev
  ```

- curlpp

  ```
  apt install libcurlpp-dev
  ```

- libjpeg

  ```
  apt install libjpeg-dev
  ```

- libpng

  ```
  apt install libpng-dev
  ```

- libtiff

  ```
  apt install libtiff-dev
  ```

- libgtk-3

  ```
  apt install libgtk-3-dev
  ```

You have to specify the version to build:

Linux & Win:

`AVX=1 chalet c -c Release`

`AVX=1 chalet b sd.ui`

`AVX=1 chalet r sd.ui`

On linux, you have to link the builded stable-diffusion_xxx.so into the /var/lib directory. 