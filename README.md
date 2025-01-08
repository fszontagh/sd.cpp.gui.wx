
## **sd.cpp.gui.wx - Stable Diffusion GUI**

A cross-platform GUI for [Stable Diffusion C++](https://github.com/leejet/stable-diffusion.cpp), built using [wxWidgets](https://www.wxwidgets.org/).

Licensed under the MIT License.

### **Features**

-   **Text-to-image (text2img)** generation
-   **Image-to-image (img2img)** generation
-   **Inpaint & Outpaint**, save / load masks, built-in inpaint / outpaint editor
-   **Tiling** for low memory
-   Model **quantization** in-fly or manually
-   Built-in **upscaling** capabilities
-   **ControlNet** integration
-   [**Model conversion** to GGUF format](https://github.com/fszontagh/sd.cpp.gui.wx/wiki/Convert-safetensors-model-into-gguf-(aka.-quantization))
-   Optional integration with [CivitAi Model Downloader](https://github.com/fszontagh/sd.cpp.gui.wx/wiki/CivitAi-Integration)
-   Hardware detection for optimal diffusion performance
-   Integrated **model management** tools
-   **VAE** support
-   **TAESD** support
-   Customizable **generation presets**
-   ComfyUI compatible **Prompt presets **
-   **Generation queue** for batch processing
-   Real-time **progress monitoring** during generation
-   Save and load metadata directly from images
-   Multi-language GUI support

#### **Supported Backends**

-   CPU (AVX, AVX2, AVX512)
-   CUDA
-   VULKAN

----------

### **OS Compatibility**

Refer to the [GitHub Releases](https://github.com/fszontagh/sd.cpp.gui.wx/releases) page for available installers.  
For compiling on unsupported operating systems, consult the Wiki.


---

## Star History

[![Star History Chart](https://api.star-history.com/svg?repos=fszontagh/sd.cpp.gui.wx&type=Date)](https://star-history.com/#fszontagh/sd.cpp.gui.wx&Date)

----------

_Use at your own risk!_  
These releases are preview versions and may contain bugs. If you encounter issues, please report them via the [GitHub Issues](https://github.com/fszontagh/sd.cpp.gui.wx/issues) page.
