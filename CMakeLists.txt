cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "sd.ui")

project(${PROJECT_NAME})

set (CMAKE_CXX_STANDARD 20)



option(SD_CUBLAS                     "sd: cuda backend" ON)
option(SD_HIPBLAS                    "sd: rocm backend" OFF)
option(SD_METAL                      "sd: metal backend" OFF)

IF(MSVC)
    SET(OPTIONS WIN32)
ENDIF(MSVC)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(${PROJECT_NAME} ${OPTIONS} main.cpp ui/MainWindow.cpp ui/MainWindowSettings.cpp ui/MainWindowUi.cpp ui/MainWindowImageViewer.cpp ui/QueueManager.cpp)


set(STABLE_DIFFUSION_DIR "${CMAKE_SOURCE_DIR}/external/stable-diffusion.cpp")
include_directories(${STABLE_DIFFUSION_DIR}/include)
link_directories(${STABLE_DIFFUSION_DIR}/CUDA/${CMAKE_BUILD_TYPE})


IF(MSVC)

    
     set(CUDA_PATH "$ENV{CUDA_PATH}")

    #set(STABLE_DIFFUSION_LIB ${CMAKE_SOURCE_DIR}/external/stable-diffusion.cpp/${CMAKE_BUILD_TYPE}/stable-diffusion.lib)
    #set(GGML_LIB ${CMAKE_SOURCE_DIR}/external/ggml/${CMAKE_BUILD_TYPE}/lib/ggml.lib)
    #set(STABLE_DIFFUSION_DLL ${CMAKE_SOURCE_DIR}/external/stable-diffusion.cpp/${CMAKE_BUILD_TYPE}/stable-diffusion.dll)
    #set(GGML_DLL ${CMAKE_SOURCE_DIR}/external/ggml/${CMAKE_BUILD_TYPE}/ggml.dll)
    
    #add_library(SDLIB SHARED IMPORTED)
    #set_property(TARGET SDLIB PROPERTY IMPORTED_LOCATION "${STABLE_DIFFUSION_DLL}")
    #set_property(TARGET SDLIB PROPERTY IMPORTED_IMPLIB "${STABLE_DIFFUSION_LIB}")

    #add_library(GGMLLIB SHARED IMPORTED)
    #set_property(TARGET GGMLLIB PROPERTY IMPORTED_LOCATION "${GGML_DLL}")
    #set_property(TARGET GGMLLIB PROPERTY IMPORTED_IMPLIB "${GGML_LIB}")
        
    add_library(stable-diffusion SHARED IMPORTED)
    add_library(ggml SHARED IMPORTED)
    
    set_target_properties(stable-diffusion PROPERTIES
    IMPORTED_CONFIGURATIONS        "DEBUG;RELEASE"
    IMPORTED_LOCATION_DEBUG        ${STABLE_DIFFUSION_DIR}/CUDA/Debug/stable-diffusion.dll
    IMPORTED_IMPLIB_DEBUG          ${STABLE_DIFFUSION_DIR}/CUDA/Debug/stable-diffusion.lib
    IMPORTED_LOCATION_RELEASE      ${STABLE_DIFFUSION_DIR}/CUDA/Release/stable-diffusion.dll
    IMPORTED_IMPLIB_RELEASE        ${STABLE_DIFFUSION_DIR}/CUDA/Release/stable-diffusion.lib
    INTERFACE_INCLUDE_DIRECTORIES  ${STABLE_DIFFUSION_DIR}/include
    )

    set_target_properties(ggml PROPERTIES
    IMPORTED_CONFIGURATIONS        "DEBUG;RELEASE"
    IMPORTED_LOCATION_DEBUG        ${STABLE_DIFFUSION_DIR}/CUDA/Debug/ggml.dll
    IMPORTED_IMPLIB_DEBUG          ${STABLE_DIFFUSION_DIR}/CUDA/Debug/ggml.lib
    IMPORTED_LOCATION_RELEASE      ${STABLE_DIFFUSION_DIR}/CUDA/Release/ggml.dll
    IMPORTED_IMPLIB_RELEASE        ${STABLE_DIFFUSION_DIR}/CUDA/Release/ggml.lib
    )
    

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${STABLE_DIFFUSION_DIR}/CUDA/${CMAKE_BUILD_TYPE}/stable-diffusion.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)                 # <--this is out-file path0
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${STABLE_DIFFUSION_DIR}/CUDA/${CMAKE_BUILD_TYPE}/ggml.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)                 # <--this is out-file path0

        set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/share/opencv4")

ENDIF(MSVC)

#set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/share/opencv4")

find_package(OpenCV REQUIRED)

if( OpenCV_FOUND )
  # Additional Include Directories
  include_directories( ${OpenCV_INCLUDE_DIRS} )

  # Additional Library Directories
  link_directories( ${OpenCV_LIB_DIR} )

  # Additional Dependencies
  # target_link_libraries(sd.ui ${OpenCV_LIBS} )
endif()

find_package(restclient-cpp CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CUDAToolkit REQUIRED)

#set(CUDA_LIB_PATH "C:/CUDA/lib/x64")


target_compile_definitions(${PROJECT_NAME} PRIVATE ${wxWidgets_DEFINITIONS} "$<$<CONFIG:DEBUG>:${wxWidgets_DEFINITIONS_DEBUG}>")
target_include_directories(${PROJECT_NAME} PRIVATE ${wxWidgets_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${CUDA_LIBRARIES})

target_link_libraries(${PROJECT_NAME} PRIVATE stable-diffusion ggml restclient-cpp opencv_ml opencv_dnn opencv_core opencv_flann nlohmann_json::nlohmann_json fmt::fmt wx::core wx::base wx::xrc wx::adv wx::richtext wx::aui CUDA::cudart CUDA::cublas CUDA::cublasLt CUDA::cuda_driver)

include_directories(ui)




