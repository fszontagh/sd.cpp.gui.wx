cmake_minimum_required(VERSION 3.7...3.16 FATAL_ERROR)

project(StableDiffusionGUI 
LANGUAGES CXX C 
VERSION 0.2.0
DESCRIPTION "Stable Diffusion C++ Desktop Gui"
HOMEPAGE_URL "https://github.com/fszontagh/sd.cpp.gui.wx"

)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_BINARY_NAME "stablediffusiongui")
set(PROJECT_DISPLAY_NAME "Stable Diffusion GUI")
# in github workflow to minimize output..
add_definitions(-w)

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

# CPU Features dependency
FetchContent_Declare(
    cpu_features
    GIT_REPOSITORY https://github.com/google/cpu_features.git
    GIT_TAG v0.9.0
)
set(BUILD_TESTING OFF)

FetchContent_MakeAvailable(cpu_features)

execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message("Git hash: ${GIT_HASH}")
message("CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR}")
message("CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}")

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
    src/ui/QueueManager.cpp
    src/ui/ModelInfo.cpp
    src/libs/SharedMemoryManager.cpp
)

if (WIN32)
    list(APPEND SOURCES minimal.rc)
endif()

add_subdirectory(extprocess)

# Set target for the application
add_executable(${PROJECT_BINARY_NAME} ${SOURCES})
target_precompile_headers(${PROJECT_BINARY_NAME} PRIVATE src/pch.h)

if (NOT WIN32)
	set(APPDEPENDS wx::base wx::core wx::xrc wx::aui wx::richtext)
else()
	set(APPDEPENDS wxWidgets::wxWidgets)
    if (MSVC)
        set_target_properties(${PROJECT_BINARY_NAME} PROPERTIES
            COMPILE_FLAGS "/DwxUSE_RC_MANIFEST"
            LINK_FLAGS "/MANIFEST:NO"
        )
    endif()
endif()


if (SDGUI_AVX)
  list(APPEND APPDEPENDS stable_diffusion_cpp_avx)
endif()

if (SDGUI_AVX2)
  list(APPEND APPDEPENDS stable_diffusion_cpp_avx2)
endif()

if(SDGUI_AVX512)
  list(APPEND APPDEPENDS stable_diffusion_cpp_avx512)
endif()

if(SDGUI_CUBLAS)
  list(APPEND APPDEPENDS stable_diffusion_cpp_cuda)
endif()

if(SDGUI_VULKAN)
  list(APPEND APPDEPENDS stable_diffusion_cpp_vulkan)
endif()

if (SDGUI_HIPBLAS)
  list(APPEND APPDEPENDS stable_diffusion_cpp_hipblas)
endif()


list(APPEND APPDEPENDS cpu_features OpenSSL::Crypto CURL::libcurl)

if (WIN32)
    list(APPEND APPDEPENDS exiv2lib)
else()
    list(APPEND APPDEPENDS exiv2lib)
endif()

include(cmake/intl.cmake)

add_dependencies(${PROJECT_BINARY_NAME} ${APPDEPENDS} po-compile)

# Include directories
target_include_directories(${PROJECT_BINARY_NAME} PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${stable_diffusion_SOURCE_DIR}
    ${cpu_features_SOURCE_DIR}
    ${wxWidgets_SOURCE_DIR}/include
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

include(cmake/stable_diffusion.cmake)
include(cmake/cpack.cmake)
