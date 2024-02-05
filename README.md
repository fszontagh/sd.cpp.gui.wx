## sd.cpp.gui.wx

wxWidgets based Stable Diffusion C++ GUi

Created by Ferenc Szontágh <szf@fsociety.hu> @2024-01-30

No licence yet... 

### - Prepare

* install vcpkg on your system... 
* configure the vcpkg in CMakePresets.json and CMakeUserPresets.json
* run configure, itt will be ~15mins to download and configure the dependencies

### - Externals!

in the external forlder, you can find the precompuled stable-diffusion.cpp and ggml as submodulde to it. 
The default libs are configured with only CUDA, the release is Debug and Release. To use with another version (for example cpu only), you need to compile and overwrite 

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
See @git.spamming.hu ticketing system

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