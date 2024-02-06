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


IF(SD_CUBLAS)
set(STABLE_DIFFUSION_DIR "${CMAKE_SOURCE_DIR}/external/stable-diffusion.cpp/CUDA")
ELSEIF(NOT SD_CUBLAS)
set(STABLE_DIFFUSION_DIR "${CMAKE_SOURCE_DIR}/external/stable-diffusion.cpp/CPU")
ENDIF()

include_directories(${CMAKE_SOURCE_DIR}/external/stable-diffusion.cpp/include)


link_directories(${STABLE_DIFFUSION_DIR}/${CMAKE_BUILD_TYPE})


IF(MSVC)

    if(SD_CUBLAS)
    set(CUDA_PATH "$ENV{CUDA_PATH}")
    endif(SD_CUBLAS)

    add_library(stable-diffusion SHARED IMPORTED)
    add_library(ggml SHARED IMPORTED)
    
    set_target_properties(stable-diffusion PROPERTIES
    IMPORTED_CONFIGURATIONS        "DEBUG;RELEASE"
    IMPORTED_LOCATION_DEBUG        ${STABLE_DIFFUSION_DIR}/Debug/stable-diffusion.dll
    IMPORTED_IMPLIB_DEBUG          ${STABLE_DIFFUSION_DIR}/Debug/stable-diffusion.lib
    IMPORTED_LOCATION_RELEASE      ${STABLE_DIFFUSION_DIR}/Release/stable-diffusion.dll
    IMPORTED_IMPLIB_RELEASE        ${STABLE_DIFFUSION_DIR}/Release/stable-diffusion.lib
    INTERFACE_INCLUDE_DIRECTORIES  ${CMAKE_SOURCE_DIR}/external/stable-diffusion.cpp/include
    )

    set_target_properties(ggml PROPERTIES
    IMPORTED_CONFIGURATIONS        "DEBUG;RELEASE"
    IMPORTED_LOCATION_DEBUG        ${STABLE_DIFFUSION_DIR}/Debug/ggml.dll
    IMPORTED_IMPLIB_DEBUG          ${STABLE_DIFFUSION_DIR}/Debug/ggml.lib
    IMPORTED_LOCATION_RELEASE      ${STABLE_DIFFUSION_DIR}/Release/ggml.dll
    IMPORTED_IMPLIB_RELEASE        ${STABLE_DIFFUSION_DIR}/Release/ggml.lib
    )
    

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${STABLE_DIFFUSION_DIR}/${CMAKE_BUILD_TYPE}/stable-diffusion.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)                 # <--this is out-file path0
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${STABLE_DIFFUSION_DIR}/${CMAKE_BUILD_TYPE}/ggml.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)                 # <--this is out-file path0

        set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/share/opencv4")

ENDIF(MSVC)

find_package(OpenCV REQUIRED)

if( OpenCV_FOUND )
  include_directories( ${OpenCV_INCLUDE_DIRS} )
  link_directories( ${OpenCV_LIB_DIR} )
endif()

find_package(restclient-cpp CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
IF(SD_CUBLAS)
find_package(CUDAToolkit REQUIRED)
ENDIF()

target_compile_definitions(${PROJECT_NAME} PRIVATE ${wxWidgets_DEFINITIONS} "$<$<CONFIG:DEBUG>:${wxWidgets_DEFINITIONS_DEBUG}>")
target_include_directories(${PROJECT_NAME} PRIVATE ${wxWidgets_INCLUDE_DIRS})



IF(SD_CUBLAS)
set(CUDA_MODULES "CUDA::cudart CUDA::cublas CUDA::cublasLt CUDA::cuda_driver")
target_link_libraries(${PROJECT_NAME} ${CUDA_LIBRARIES})
else()
set(CUDA_MODULES "")
ENDIF()

target_link_libraries(${PROJECT_NAME} PRIVATE stable-diffusion ggml restclient-cpp opencv_ml opencv_dnn opencv_core opencv_flann nlohmann_json::nlohmann_json fmt::fmt wx::core wx::base wx::xrc wx::adv wx::richtext wx::aui ${CUDA_MODULES})

include_directories(ui)




