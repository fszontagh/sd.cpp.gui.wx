cmake_minimum_required(VERSION 3.7...3.16 FATAL_ERROR)

project(StableDiffusionGUI 
VERSION 0.1.7 
DESCRIPTION "Stable Diffusion C++ Desktop Gui"
HOMEPAGE_URL "https://github.com/fszontagh/sd.cpp.gui.wx"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(ExternalProject)

set(BUILD_SHARED_LIBS OFF)
set(OPENSSL_USE_STATIC_LIBS TRUE)

if (WIN32)
    find_package(OpenSSL CONFIG REQUIRED)
    find_package(CURL CONFIG REQUIRED)
    find_package(Exiv2 CONFIG REQUIRED)
else()
    # on ubuntu, there is no CONFIG
    find_package(OpenSSL REQUIRED)
    find_package(CURL REQUIRED)
    find_package(exiv2 REQUIRED)
endif(WIN32)



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

configure_package_config_file(src/ver.hpp.in ver.hpp
    INSTALL_DESTINATION build/install
)

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
)

# Set target for the application
add_executable(sd_ui ${SOURCES})


set(APPDEPENDS wx::base wx::core wx::xrc wx::aui wx::richtext)

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

if (SDGUI_ROCM)
  list(APPEND APPDEPENDS stable_diffusion_cpp_rocm)
endif()


list(APPEND APPDEPENDS cpu_features OpenSSL::Crypto CURL::libcurl)

if (WIN32)
    list(APPEND APPDEPENDS exiv2::exiv2)
    #add_dependencies(sd_ui cpu_features OpenSSL::Crypto CURL::libcurl exiv2::exiv2)
else()
    #add_dependencies(sd_ui cpu_features OpenSSL::Crypto CURL::libcurl exiv2lib)
    list(APPEND APPDEPENDS exiv2lib)
endif()


add_dependencies(sd_ui ${APPDEPENDS})

# Include directories
target_include_directories(sd_ui PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${stable_diffusion_SOURCE_DIR}
    ${cpu_features_SOURCE_DIR}
    ${wxWidgets_SOURCE_DIR}/include
)


if(WIN32)
    target_link_libraries(sd_ui PRIVATE
    cpu_features
    wx::base wx::core wx::xrc wx::aui wx::richtext
    OpenSSL::Crypto CURL::libcurl exiv2::exiv2
    )

else(WIN32)

target_link_libraries(sd_ui PRIVATE
    cpu_features
    wx::base wx::core wx::xrc wx::aui wx::richtext
    OpenSSL::Crypto CURL::libcurl exiv2lib
)

endif(WIN32)



# Windows-specific options
if(WIN32)
    target_link_libraries(sd_ui PRIVATE comsuppw.lib wbemuuid.lib)
    target_compile_definitions(sd_ui PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _UNICODE
        UNICODE
        WIN32
        _WINDOWS
        wxUSE_RC_MANIFEST=1
        __WXMSW__
    )
    set_property(TARGET sd_ui PROPERTY WIN32_EXECUTABLE TRUE)
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
    target_compile_definitions(sd_ui PRIVATE
        __WXGTK__
        _FILE_OFFSET_BITS=64
    )
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    target_include_directories(sd_ui PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(sd_ui PRIVATE ${GTK3_LIBRARIES})
endif()

include(cmake/stable_diffusion.cmake)
include(cmake/cpack.cmake)
