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


### Compatibility

Please see [github releases](https://github.com/fszontagh/sd.cpp.gui.wx/releases) for installers. These win packages are tested on Windows 11, .deb packages tested on Ubuntu 24

_Use only at your own risk!_
These releases just previews, so crashes may occur. For bugreport, please use [github Issuses](https://github.com/fszontagh/sd.cpp.gui.wx/issues).

### Starting with selected backend
To select a backend on startup, add one from the following parameters:
 - -cuda
 - -avx
 - -avx2
 - -avx512
 - hipblas
 - vulkan
 
 To allow to run in multi - instance mode, start with ```-allow-multiple``` argument. Otherwise only one instance is allowed to run in

### Building on Linux / Windows

#### Requirements - Windows

- latest [conan]() for package dependencies (```winget install -e --id JFrog.Conan```)
- latest [cmake](https://cmake.org/) (```winget install -e --id Kitware.CMake```)
- a compatible compiler (Visual Studio Community 17 2022) (```winget install --id=Microsoft.VisualStudio.2022.Community  -e```)
- a git client (```winget install -e --id Git.Git```)
- Optional NSIS for creating windows installer (```winget install -e --id NSIS.NSIS```)

#### Compile on Windows - pre-requirements

 Open a developer terminal, then run:
 ```
 git clone https://github.com/fszontagh/sd.cpp.gui.wx.git
 mkdir build
 cd build
 ```
Run conan to gather dependencies:
```
conan install ..
```

#### Requirements - Linux
- build tools

  ```
  apt install build-essential
  ```

- exiv2

  ```
  apt install libexiv2-dev
  ```

- OpenSSL:

  ```
  apt install libssl-dev
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
- libcurl

  ```
  apt install libcurl4-openssl-dev
  ```
- libsecret-1-dev

  ```
  apt install libsecret-1-dev
  ```
- libnotify-dev

  ```
  apt install libnotify-dev
  ```
- libwebkit2gtk-4.1-dev

  ```
  apt install libwebkit2gtk-4.1-dev
  ```
- libsdl2-dev

  ```
  apt install libsdl2-dev
  ```

#### Configure the project: 
Run in the build folder:
```
cmake .. 
```
If you configure without any parameter, the sd.cpp libs will not be compiled. You need to specify the correct backend to compile. For CPU only, please checkout whitch cpu features are available on your platform. 
For AVX only: 
```
cmake .. -DSD_AVX=ON
```

For AVX2 only:
```
cmake .. -DSD_AVX2=ON
```

For AVX512
```
cmake .. -DSD_AVX512=ON
```

For CUDA only:
```
cmake .. -DSD_CUBLAS=ON
```
For VULKAN only:
```
cmake .. -DSD_VULKAN=ON
```
For HIPBLAS only:
```
cmake .. -DSD_HIPBLAS=ON
```
It is possible to combine more parameters 

```
cmake .. -DSD_CUBLAS=ON -DSD_AVX=ON -DSD_AVX2=ON -DSD_AVX512=OFF
```

Of course, for VGA backends, need to be install the required tools. 

To compile, run:
```
cmake --build . --config Release
```

To create package:
```
cmake --build . --config Release --target package
```