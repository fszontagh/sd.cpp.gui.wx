cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "sd.ui")

project(${PROJECT_NAME})

set (CMAKE_CXX_STANDARD 20)

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
option(SD_CUBLAS                     "sd: cuda backend" ON)
option(SD_HIPBLAS                    "sd: rocm backend" OFF)
option(SD_METAL                      "sd: metal backend" OFF)
option(SD_FLASH_ATTN                 "sd: use flash attention for x4 less memory usage" OFF)
option(BUILD_SHARED_LIBS             "sd: build shared libs" OFF)
#option(SD_BUILD_SERVER               "sd: build server example"                           ON)


# stolen ggml options..
# instruction set specific
option(GGML_AVX                     "ggml: enable AVX"                                     ON)
option(GGML_AVX2                    "ggml: enable AVX2"                                    ON)
option(GGML_AVX512                  "ggml: enable AVX512"                                  OFF)
option(GGML_AVX512_VBMI             "ggml: enable AVX512-VBMI"                             OFF)
option(GGML_AVX512_VNNI             "ggml: enable AVX512-VNNI"                             OFF)
option(GGML_FMA                     "ggml: enable FMA"                                     ON)
# in MSVC F16C is implied with AVX2/AVX512
if (NOT MSVC)
    option(GGML_F16C                "ggml: enable F16C"                                    ON)
endif()



include(external/stable-diffusion.cmake)


if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(${PROJECT_NAME} ${OPTIONS} main.cpp ui/MainWindow.cpp ui/MainWindowSettings.cpp ui/MainWindowUi.cpp ui/MainWindowImageViewer.cpp ui/QueueManager.cpp)


#add_dependencies(${PROJECT_NAME} stable-diffusion)

#include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
#link_directories(${EXTERNAL_INSTALL_LOCATION}/bin)

IF(WIN32)

    if(SD_CUBLAS)
    set(CUDA_PATH "$ENV{CUDA_PATH}")
    message("-- CUDA_PATH=" ${CUDA_PATH})
    find_package(CUDAToolkit REQUIRED)
    endif(SD_CUBLAS)
    set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/share/opencv4")



    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/${CMAKE_BUILD_TYPE}/stable-diffusion.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/${CMAKE_BUILD_TYPE}/ggml.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)


    IF(CMAKE_BUILD_TYPE EQUAL "Debug")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/${CMAKE_BUILD_TYPE}/stable-diffusion.pdb
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/${CMAKE_BUILD_TYPE}/ggml.pdb
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)    
    ENDIF()


ENDIF(WIN32)

find_package(OpenCV REQUIRED)

if( OpenCV_FOUND )
  include_directories( ${OpenCV_INCLUDE_DIRS} )
  link_directories( ${OpenCV_LIB_DIR} )
endif()

find_package(restclient-cpp CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)




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

target_link_libraries(${PROJECT_NAME} PRIVATE stable-diffusionlib ggmllib CUDA::cudart CUDA::cublas CUDA::cublasLt CUDA::cuda_driver restclient-cpp opencv_ml opencv_dnn opencv_core opencv_flann nlohmann_json::nlohmann_json fmt::fmt wx::core wx::base wx::xrc wx::adv wx::richtext wx::aui)

else()
set(CUDA_MODULES "")
target_link_libraries(${PROJECT_NAME} PRIVATE stable-diffusionlib ggmllib restclient-cpp opencv_ml opencv_dnn opencv_core opencv_flann nlohmann_json::nlohmann_json fmt::fmt wx::core wx::base wx::xrc wx::adv wx::richtext wx::aui)
ENDIF()





include_directories(ui)




