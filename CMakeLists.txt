cmake_minimum_required(VERSION 3.15)
project(StableDiffusionGUI VERSION 0.1.7 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo;RelHighOpt;Profile;DebugSanitize;DebugSanitizeAddress;DebugSanitizeThread;DebugSanitizeMemory;DebugSanitizeLeak;DebugSanitizeUndefined;DebugSanitizeHW;DebugSanitizeHWAddress;ReleaseSanitizeUndefined" CACHE STRING "Configurations" FORCE)

# External dependencies
include(FetchContent)

include(cmake/stable_diffusion.cmake)

# CPU Features dependency
FetchContent_Declare(
    cpu_features
    GIT_REPOSITORY https://github.com/google/cpu_features.git
    GIT_TAG v0.9.0
)
FetchContent_MakeAvailable(cpu_features)

set(CURLPP_BUILD_SHARED_LIBS OFF)
FetchContent_Declare(
    curlpp
    GIT_REPOSITORY https://github.com/jpbarrette/curlpp.git
    GIT_TAG v0.8.1
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(curlpp)


if(NOT curlpp_POPULATED)
   FetchContent_Populate(curlpp)
   add_subdirectory(${curlpp_SOURCE_DIR} ${curlpp_BUILD_DIR} EXCLUDE_FROM_ALL)
endif()

message(STATUS "curlpp_SOURCE_DIR: ${curlpp_SOURCE_DIR}")
# wxWidgets dependency

include(cmake/wxWidgets.cmake)

ExternalProject_Add(
  exiv2
  GIT_REPOSITORY https://github.com/Exiv2/exiv2.git
  GIT_TAG v0.27.0
  CMAKE_ARGS -DEXIV2_BUILD_SAMPLES=OFF -DBUILD_SHARED_LIBS=OFF -DEXIV2_BUILD_UNIT_TESTS=OFF -DEXIV2_BUILD_EXIV2_COMMAND=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/exiv2-install
  BUILD_BYPRODUCTS <INSTALL_DIR>/lib/${CMAKE_FIND_LIBRARY_PREFIXES}xmp${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib/${CMAKE_FIND_LIBRARY_PREFIXES}exiv2${CMAKE_STATIC_LIBRARY_SUFFIX}
  INSTALL_DIR ${CMAKE_BINARY_DIR}/exiv2-install
  EXCLUDE_FROM_ALL TRUE
)

set(EXIV2_INCLUDE_DIR ${CMAKE_BINARY_DIR}/exiv2-install/include)
set(EXIV2_LIBRARY ${CMAKE_BINARY_DIR}/exiv2-install/lib/${CMAKE_FIND_LIBRARY_PREFIXES}exiv2${CMAKE_STATIC_LIBRARY_SUFFIX})
set(XMP_LIBRARY ${CMAKE_BINARY_DIR}/exiv2-install/lib/${CMAKE_FIND_LIBRARY_PREFIXES}xmp${CMAKE_STATIC_LIBRARY_SUFFIX})


message("EXIV2_INCLUDE_DIR: ${EXIV2_INCLUDE_DIR}")
message("EXIV2_LIBRARY: ${EXIV2_LIBRARY}")
message("XMP_LIBRARY: ${XMP_LIBRARY}")

configure_package_config_file(src/ver.hpp.in ver.hpp
    INSTALL_DESTINATION build/install
)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

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


include(cmake/cpack.cmake)

# Include directories
target_include_directories(sd_ui PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${stable_diffusion_SOURCE_DIR}
    ${cpu_features_SOURCE_DIR}
    ${wxWidgets_SOURCE_DIR}/include
    ${curlpp_SOURCE_DIR}/include
)

# Linking libraries
target_link_libraries(sd_ui PRIVATE
    cpu_features
    wx::base wx::core wx::xrc wx::aui
    curlpp_static
    OpenSSL::Crypto
    ${EXIV2_LIBRARY} ${XMP_LIBRARY}
)

# Windows-specific options
if(WIN32)
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


# Add a target for Git hash script
if(WIN32)
    add_custom_target(get_git_hash ALL
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/githash.bat ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Getting Git hash on Windows"
    )
else()
    add_custom_target(get_git_hash ALL
        COMMAND bash ${CMAKE_SOURCE_DIR}/scripts/githash.sh ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Getting Git hash on non-Windows systems"
    )
endif()

