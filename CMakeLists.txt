cmake_minimum_required(VERSION 3.7...3.16 FATAL_ERROR)

project(StableDiffusionGUI
LANGUAGES CXX C
VERSION 0.2.7
DESCRIPTION "Stable Diffusion C++ Desktop Gui"
HOMEPAGE_URL "https://github.com/fszontagh/sd.cpp.gui.wx"
)

set(PROJECT_AUTHOR_NAME "Ferenc Szontágh")
set(PROJECT_AUTHOR_EMAIL "szf@fsociety.hu")
set(PROJECT_AUTHOR "${PROJECT_AUTHOR_NAME} <${PROJECT_AUTHOR_EMAIL}>")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(cmake/sdcpp_version.cmake)
include(cmake/llama_version.cmake)
include(cmake/wxWidgets_version.cmake)

set(PROJECT_BINARY_NAME "stablediffusiongui")
set(PROJECT_DISPLAY_NAME "Stable Diffusion GUI")
# in github workflow to minimize output..
if (DEFINED ENV{GITHUB_ACTIONS})
    set(CMAKE_VERBOSE_MAKEFILE OFF)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
endif()


include(FetchContent)
include(ExternalProject)
include(CMakePackageConfigHelpers)

set(BUILD_SHARED_LIBS OFF)

if (WIN32)
    set(OPENSSL_USE_STATIC_LIBS TRUE)
endif()



find_package(OpenSSL CONFIG QUIET)
if (NOT OpenSSL_FOUND)
    find_package(OpenSSL REQUIRED)
endif()

find_package(CURL CONFIG QUIET)
if (NOT CURL_FOUND)
    find_package(CURL REQUIRED)
endif()

find_package(exiv2 CONFIG QUIET)
if (NOT exiv2_FOUND)
    find_package(exiv2 REQUIRED)
endif()


include(cmake/wxWidgets.cmake)
include(cmake/CpuFeatures.cmake)




execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "GIT HASH: ${GIT_HASH}")
message(STATUS "SD.CPP VERSION: ${SDCPP_VERSION}")
message(STATUS "LLAMA.CPP VERSION: ${LLAMA_VERSION}")

configure_file(src/ver.hpp.in ver.hpp)


# Source files
set(SOURCES
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/MainWindowAboutDialog.cpp
    src/ui/MainWindowSettings.cpp
    src/ui/MainWindowCivitAiWindow.cpp
    src/ui/MainWindowImageDialog.cpp
    src/ui/MainWindowUI.cpp
    src/ui/MainWindowDesktopWidget.cpp
    src/ui/QueueManager.cpp
    src/ui/ModelInfo.cpp
    src/libs/SharedMemoryManager.cpp
    src/libs/TcpClient.cpp
    src/helpers/QueueItem.cpp
    src/helpers/DataViewListManager.cpp
    src/helpers/ModelUiManager.cpp
)

if (WIN32)
    set(BNAME ${PROJECT_BINARY_NAME})
    set(COMPONENT_NAME ${PROJECT_DISPLAY_NAME})
    configure_file(platform/msvc/app.rc.in app.rc)
    configure_file(platform/msvc/minimal.rc.in minimal.rc)
    list(APPEND SOURCES ${CMAKE_BINARY_DIR}/app.rc)
    list(APPEND SOURCES ${CMAKE_BINARY_DIR}/minimal.rc)
endif()

add_subdirectory(extprocess)

option(SD_SERVER "Build socket server" OFF)



# Set target for the application
add_executable(${PROJECT_BINARY_NAME} ${SOURCES})
target_precompile_headers(${PROJECT_BINARY_NAME} PRIVATE src/pch.h)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${PROJECT_BINARY_NAME} PRIVATE -Wunused)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${PROJECT_BINARY_NAME} PRIVATE /w44265)
    endif()
endif()


if (NOT WIN32)

set(APPDEPENDS wx::base wx::core wx::xrc wx::richtext wx::stc)

else()
    set(APPDEPENDS wxWidgets::wxWidgets)
    if (MSVC)
        set_target_properties(${PROJECT_BINARY_NAME} PROPERTIES
            COMPILE_FLAGS "/DwxUSE_RC_MANIFEST"
            LINK_FLAGS "/MANIFEST:NO"
        )
    endif()
endif()

include(cmake/sockets-cpp.cmake)

if (SD_SERVER)
    add_subdirectory(server)
endif()


if (SDGUI_AVX)
  list(APPEND APPDEPENDS stable_diffusion_cpp_avx llama_avx)
endif()

if (SDGUI_AVX2)
  list(APPEND APPDEPENDS stable_diffusion_cpp_avx2 llama_avx2)
endif()

if(SDGUI_AVX512)
  list(APPEND APPDEPENDS stable_diffusion_cpp_avx512 llama_avx512)
endif()

if(SDGUI_CUBLAS)
  list(APPEND APPDEPENDS stable_diffusion_cpp_cuda llama_cuda)
endif()

if(SDGUI_VULKAN)
  list(APPEND APPDEPENDS stable_diffusion_cpp_vulkan llama_vulkan)
endif()

if (SDGUI_HIPBLAS)
  list(APPEND APPDEPENDS stable_diffusion_cpp_hipblas llama_hipblas)
endif()


list(APPEND APPDEPENDS CpuFeatures::cpu_features OpenSSL::Crypto CURL::libcurl exiv2lib)

include(cmake/intl.cmake)

add_dependencies(${PROJECT_BINARY_NAME} ${APPDEPENDS} po-compile sockets_cpp)

# Include directories
target_include_directories(${PROJECT_BINARY_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CpuFeatures_SOURCE_DIR}
    ${wxWidgets_SOURCE_DIR}/include
    ${sockets_cpp_SOURCE}
)
message(STATUS "APPDEPS: ${APPDEPENDS}")


target_link_libraries(${PROJECT_BINARY_NAME} PRIVATE ${APPDEPENDS})



# Windows-specific options
if(WIN32)
    target_link_libraries(${PROJECT_BINARY_NAME} PRIVATE comsuppw.lib wbemuuid.lib)
    target_compile_definitions(${PROJECT_BINARY_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
       _UNICODE
        UNICODE
        WIN32
        _WINDOWS
        wxUSE_RC_MANIFEST=1
        __WXMSW__
    )
    set_property(TARGET ${PROJECT_BINARY_NAME} PROPERTY WIN32_EXECUTABLE TRUE)
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
    )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif()

# Linux-specific options
if(UNIX AND NOT APPLE)
    target_compile_definitions(${PROJECT_BINARY_NAME} PRIVATE
        __WXGTK__
        _FILE_OFFSET_BITS=64
    )
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    target_include_directories(${PROJECT_BINARY_NAME} PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_BINARY_NAME} PRIVATE ${GTK3_LIBRARIES})
endif()


option(SD_AVX "Enable AVX library build" OFF)
option(SD_AVX2 "Enable AVX2 library build" OFF)
option(SD_AVX512 "Enable AVX512 library build" OFF)
option(SD_CUDA "Enable CUDA library build" OFF)
option(SD_HIPBLAS "Enable HIPBLAS library build" OFF)
option(SD_VULKAN "Enable Vulkan library build" OFF)

if (SD_AVX)
    set(SDGUI_AVX ON)
endif()


if (SD_AVX2)
    set(SDGUI_AVX2 ON)
endif()

if (SD_AVX512)
    set(SDGUI_AVX512 ON)
endif()


if (SD_CUDA)
    set(SDGUI_CUBLAS ON)
endif()


if (SD_HIPBLAS)
    set(SDGUI_HIPBLAS ON)
endif()


if (SD_VULKAN)
    set(SDGUI_VULKAN ON)
endif()


include(cmake/stable_diffusion.cmake)
include(cmake/llama.cmake)
include(cmake/cpack.cmake)
