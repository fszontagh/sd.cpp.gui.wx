cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "sd.ui")

project(${PROJECT_NAME})



#https://stackoverflow.com/questions/47690822/possible-to-force-cmake-msvc-to-use-utf-8-encoding-for-source-files-without-a-bo
#nlohmann json utf-8 and windows filename
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_SOURCE_DIR}/external)
set(EXTERNAL_DOWNLOAD_LOCATION ${CMAKE_SOURCE_DIR}/external)


IF(WIN32)
    SET(OPTIONS WIN32)
ENDIF(WIN32)

#
# Option list
#

# general
#option(SD_BUILD_TESTS                "sd: build tests"    ${SD_STANDALONE})
option(SD_BUILD_EXAMPLES             "sd: build examples" ${SD_STANDALONE})
option(SD_CUBLAS                     "sd: cuda backend" OFF)
option(SD_HIPBLAS                    "sd: rocm backend" OFF)
option(SD_METAL                      "sd: metal backend" OFF)
option(SD_FLASH_ATTN                 "sd: use flash attention for x4 less memory usage" OFF)
option(BUILD_SHARED_LIBS             "sd: build shared libs" OFF)
#option(SD_BUILD_SERVER               "sd: build server example"                           ON)


# stolen ggml options..
# instruction set specific
option(GGML_AVX                     "ggml: enable AVX"                                     OFF)
option(GGML_AVX2                    "ggml: enable AVX2"                                    OFF)
option(GGML_AVX512                  "ggml: enable AVX512"                                  OFF)
option(GGML_AVX512_VBMI             "ggml: enable AVX512-VBMI"                             OFF)
option(GGML_AVX512_VNNI             "ggml: enable AVX512-VNNI"                             OFF)
option(GGML_FMA                     "ggml: enable FMA"                                     ON)
# in MSVC F16C is implied with AVX2/AVX512
if (NOT MSVC)
    option(GGML_F16C                "ggml: enable F16C"                                    ON)
endif()

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug)
endif()

include(external/CMakeLists.txt)

if(WIN32)
  set(SRCS ${SRCS} minimal.rc)
endif(WIN32)

add_executable(${PROJECT_NAME} ${OPTIONS} ${SRCS} main.cpp ui/MainWindow.cpp ui/MainWindowSettings.cpp ui/MainWindowUI.cpp ui/QueueManager.cpp ui/MainWindowImageDialog.cpp ui/ModelInfo.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/src/stable-diffusion-git)

IF(WIN32)
    if(SD_CUBLAS)
    set(CUDA_PATH "$ENV{CUDA_PATH}")
    message("-- CUDA_PATH=" ${CUDA_PATH})
    find_package(CUDAToolkit REQUIRED)
    endif(SD_CUBLAS)

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/$<CONFIGURATION>/stable-diffusion.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

        #add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        #COMMAND ${CMAKE_COMMAND} -E copy_if_different
        #${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/$<CONFIGURATION>/ggml.dll
        #$<TARGET_FILE_DIR:${PROJECT_NAME}>)



    IF(CMAKE_BUILD_TYPE EQUAL "Debug")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/$<CONFIGURATION>/stable-diffusion.pdb
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

        #add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        #COMMAND ${CMAKE_COMMAND} -E copy_if_different
        #${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/$<CONFIGURATION>/ggml.pdb
        #$<TARGET_FILE_DIR:${PROJECT_NAME}>)    
    ENDIF()

ELSE()
    if(SD_CUBLAS)
    find_package(CUDAToolkit REQUIRED)
    endif(SD_CUBLAS)
ENDIF(WIN32)

find_package(fmt CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(unofficial-curlpp CONFIG REQUIRED)
find_package(exiv2 CONFIG REQUIRED)


target_compile_definitions(${PROJECT_NAME} PRIVATE ${wxWidgets_DEFINITIONS} "$<$<CONFIG:DEBUG>:${wxWidgets_DEFINITIONS_DEBUG}>")
target_include_directories(${PROJECT_NAME} PRIVATE ${wxWidgets_INCLUDE_DIRS})



IF(SD_CUBLAS)
enable_language(CUDA) 

include(FindCUDA/select_compute_arch)
CUDA_DETECT_INSTALLED_GPUS(INSTALLED_GPU_CCS_1)
string(STRIP "${INSTALLED_GPU_CCS_1}" INSTALLED_GPU_CCS_2)
string(REPLACE " " ";" INSTALLED_GPU_CCS_3 "${INSTALLED_GPU_CCS_2}")
string(REPLACE "." "" CUDA_ARCH_LIST "${INSTALLED_GPU_CCS_3}")
SET(CMAKE_CUDA_ARCHITECTURES ${CUDA_ARCH_LIST})

target_link_libraries(${PROJECT_NAME} PRIVATE stable-diffusion ggml unofficial::curlpp::curlpp OpenSSL::Crypto CUDA::cudart CUDA::cublas CUDA::cublasLt CUDA::cuda_driver nlohmann_json::nlohmann_json fmt::fmt wx::core wx::base wx::xrc wx::adv wx::richtext wx::aui wx::propgrid wx::html wx::net Exiv2::exiv2lib)

else()
set(CUDA_MODULES "")
target_link_libraries(${PROJECT_NAME} PRIVATE stable-diffusion ggml unofficial::curlpp::curlpp OpenSSL::Crypto nlohmann_json::nlohmann_json fmt::fmt wx::core wx::base wx::xrc wx::adv wx::richtext wx::aui wx::propgrid wx::html wx::net Exiv2::exiv2lib)
ENDIF()





include_directories(ui)


target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

