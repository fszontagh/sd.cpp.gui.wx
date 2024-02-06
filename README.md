## sd.cpp.gui.wx

wxWidgets based Stable Diffusion C++ GUi

Created by Ferenc Szontágh <szf@fsociety.hu> @2024-01-30

No licence yet... 

### ScreenShow!!!
#### Features: 

- text2img generation
- random seed selection by ui
- model & vae selection
- all generation config on ui (except some: tiled vae, rng generator ...)
- preset management (save / restore some config from UI)
- mode list with 1level subfolders
- queue & queue list (management WIP)
- progress bar in the queuelist, speed (it/s or s/it)
- restore last queue list, auto start last pending job

#### Some screenshots

**text2img** 

![](https://fsociety.hu/sd.cpp.gui.wx/sd.cpp.gui.wx_text2img.png)

------------

**model list** 

![](https://fsociety.hu/sd.cpp.gui.wx/sd.cpp.gui.wx_models.png)

------------
**queue management**

![](https://fsociety.hu/sd.cpp.gui.wx/sd.cpp.gui.wx_queue.png)


------------
**simple settings**

![](https://fsociety.hu/sd.cpp.gui.wx/sd.cpp.gui.wx_settings.png)

### Compatibility

Only tested on win11, yet with latest Cuda toolkit. 


### - Prepare

* install vcpkg on your system... 
* ~~configure the vcpkg in CMakePresets.json and CMakeUserPresets.json~~
* To progressbar support, clone this repo: https://github.com/fszontagh/stable-diffusion.cpp and build dlls and libs (see section: "Build stable-diffusion.cpp")

### Configure & build from commandline
Clone the repo:
```Bash
git clone https://github.com/fszontagh/sd.cpp.gui.wx.git
cd sd.cpp.gui.wx
mkdir build
```

Configure
 ```Bash
 cmake -B build/ -S . -DCMAKE_TOOLCHAIN_FILE=<vcpkg_root>/scripts/buildsystems/vcpkg.cmake 
 ```
Build
 ```Bash
 cmake --build build/
 ```

### - Externals!

Into the externals folder, place the compiled libs: 
* external/CUDA/stable-diffusion.dll 
* external/CUDA/ggml.dll

The cmake looking for it!

#### Build stable-diffusion.cpp
```Bash
git clone --recursive https://github.com/leejet/stable-diffusion.cpp
cd stable-diffusion.cpp
git pull origin master
git submodule init
git submodule update

mkdir build
cd build
#with CUDA 
cmake .. -DSD_CUBLAS=ON -DBUILD_SHARED_LIBS=ON

#build
cmake --build . --config Release
#or Debug
cmake --build . --config Debug

#test
./bin/sd -m ../models/sd-v1-4.ckpt -p "a lovely cat"
```


## - TO-DOs
* img2img
* upscaler
* details of finished jobs / previews
* civitai.com client (download model, lora, infos, previews)
* on cpu, detect the number of physical cores [issue3](https://github.com/fszontagh/sd.cpp.gui.wx/issues/3)

## - Known Bugs
* In Debug build, some BoxSizer triggering exception on settings window is open (fyi: never press the stop :) )
* The App is just exiting if an unsoported Lora / embedding is loaded, eg. CUDA out of memory. This is a stable-diffusion.cpp limitation...
* SDXL is just generating black images. See: [issues/167](https://github.com/leejet/stable-diffusion.cpp/issues/167)

## FMI
### Hash generation from binary
Repo: [okdshin/PicoSHA2](https://github.com/okdshin/PicoSHA2)

RAW file: [picosha2.h](https://raw.githubusercontent.com/okdshin/PicoSHA2/master/picosha2.h)

Example: 
```C++
std::ifstream f("file.txt", std::ios::binary);
std::vector<unsigned char> s(picosha2::k_digest_size);
picosha2::hash256(f, s.begin(), s.end());
```

### restclient-cpp

Curl wrapper to civiati api:
[repo](https://github.com/mrtazz/restclient-cpp)

installed via vcpkg as cmake dependency

[CivitAi API docs](https://github.com/civitai/civitai/wiki/REST-API-Reference#get-apiv1models-versionsby-hashhash)

### - IDE
Use the vscode... Install the extensions which is recommended by the workspace

## Table Of Thread signal tokens (W.I.P)

| thread tokens       | payload       | string | desc                                                                              | comment                                                                                               |
| ------------------- | ------------- | ------ | --------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------- |
| MODEL_LOAD_START    | QM::QueueItem |        | model load started                                                                | only occurs, when no model loaded, or the job have another model                                      |
| MODEL_LOAD_DONE     | sd_ctx\*      |        | model is loaded                                                                   | the model is loaded and the pointer returning as payload. The pointer will be reused in the next etap |
| MODEL_LOAD_ERROR    | QM::QueueItem |        | model is not loaded, something went wrong at the backend                          |
| GENERATION_ERROR    | QM::QueueItem |        |                                                                                   |                                                                                                       |
| QUEUE               | QM::QueueItem |        | simple queue msgs, the string is always a number                                  |
| GENERATION_START    | QM::QueueItem |        | the generation just started                                                       |
| GENERATION_PROGRESS |               |        | for progressbar, if sometimes the sd.cpp implements some progress handling        | not used, while no progress info from the backend                                                     |
| GENERATION_DONE     | sd_image_t \* |        | everything is done, images saved to the output. Payload is the sd_image_t pointer |

table generated with https://tabletomarkdown.com/convert-spreadsheet-to-markdown/