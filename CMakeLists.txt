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

option(SDGUI_SERVER "Build socket server" OFF)
option(SDGUI_LLAMA_DISABLE "Disable llama.cpp" OFF)
option(SDGUI_LOCAL_DISABLE "Disable local generation, this will disable the server component too" OFF)
option(SDGUI_WINDOWLESS "Build without the main application" OFF)
option(CMARK_GFM_PATH "Path to the cmark-gfm installation" "")

if (SDGUI_LOCAL_DISABLE EQUAL ON)
    set(SDGUI_SERVER OFF)
    set(SDGUI_WINDOWLESS OFF)
endif()

option(SDGUI_AVX "Enable AVX" OFF)
option(SDGUI_AVX2 "Enable AVX2" OFF)
option(SDGUI_AVX512 "Enable AVX512" OFF)
option(SDGUI_CUDA "Enable CUDA" OFF)
option(SDGUI_VULKAN "Enable VULKAN" OFF)
option(SDGUI_METAL "Enable Metal" OFF)
option(SDGUI_HIPBLAS "Enable HIPBLAS" OFF)

if (SDGUI_METAL)
   message(FATAL ERROR "Metal is not supported")
endif()

# check if all backend is disabled
if (NOT SDGUI_LOCAL_DISABLE)
    if (SDGUI_SERVER OR NOT SDGUI_WINDOWLESS)
        if (NOT SDGUI_AVX AND NOT SDGUI_AVX2 AND NOT SDGUI_AVX512 AND NOT SDGUI_CUDA AND NOT SDGUI_VULKAN AND NOT SDGUI_METAL AND NOT SDGUI_HIPBLAS)
            if (CMAKE_BUILD_TYPE STREQUAL "Debug")
                message(FATAL_ERROR "You must enable at least one backend.")
            endif()
        endif()
    endif()
endif()

if (SDGUI_WINDOWLESS AND NOT SDGUI_SERVER)
    message(WARNING "Building only libs")
endif()


message(STATUS "Build options: ")
message("\t SDGUI_AVX: \t\t${SDGUI_AVX}")
message("\t SDGUI_AVX2: \t\t${SDGUI_AVX2}")
message("\t SDGUI_AVX512: \t\t${SDGUI_AVX512}")
message("\t SDGUI_CUDA: \t\t${SDGUI_CUDA}")
message("\t SDGUI_VULKAN: \t\t${SDGUI_VULKAN}")
message("\t SDGUI_METAL: \t\t${SDGUI_METAL}")
message("\t SDGUI_HIPBLAS: \t${SDGUI_HIPBLAS}")
message("\t SDGUI_SERVER: \t\t${SDGUI_SERVER}")
message("\t SDGUI_LLAMA_DISABLE: \t${SDGUI_LLAMA_DISABLE}")
message("\t SDGUI_LOCAL_DISABLE: \t${SDGUI_LOCAL_DISABLE}")
message("\t SDGUI_WINDOWLESS: \t${SDGUI_WINDOWLESS}")

include(cmake/definitions.cmake)

include(cmake/sdcpp_version.cmake)
include(cmake/llama_version.cmake)


if (NOT SDGUI_WINDOWLESS OR SDGUI_SERVER)
    include(cmake/wxWidgets_version.cmake)
endif()
set(PROJECT_BINARY_NAME "stablediffusiongui")
set(PROJECT_DISPLAY_NAME "Stable Diffusion GUI")

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

if (NOT SDGUI_WINDOWLESS OR SDGUI_SERVER)
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
endif()
if (NOT SDGUI_WINDOWLESS)
    find_package(exiv2 CONFIG QUIET)

    if (NOT exiv2_FOUND)
        find_package(exiv2 REQUIRED)
    endif()


endif()
if (NOT SDGUI_WINDOWLESS OR SDGUI_SERVER)
    include(cmake/wxWidgets.cmake)
    include(cmake/CpuFeatures.cmake)
    include(cmake/sockets-cpp.cmake)
    include(cmake/cmark-gfm.cmake)
endif()




execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "GIT HASH: ${GIT_HASH}")

if (NOT SDGUI_LOCAL_DISABLE)
    message(STATUS "SD.CPP VERSION: ${SDCPP_VERSION}")
endif()

if (NOT SDGUI_LLAMA_DISABLE)
    message(STATUS "LLAMA.CPP VERSION: ${LLAMA_VERSION}")
endif()

configure_file(src/ver.hpp.in ver.hpp)

if (NOT SDGUI_WINDOWLESS)
    add_subdirectory(src)
endif()

if (NOT SDGUI_LOCAL_DISABLE)
    if (SDGUI_AVX OR SDGUI_AVX2 OR SDGUI_AVX512 OR SDGUI_CUDA OR SDGUI_VULKAN OR SDGUI_METAL OR SDGUI_HIPBLAS)
        include(cmake/stable_diffusion.cmake)

        if (NOT SDGUI_LLAMA_DISABLE)
            include(cmake/llama.cmake)
        endif()
    endif()


    if (NOT SDGUI_WINDOWLESS OR SDGUI_SERVER)
    add_subdirectory(extprocess)
    add_dependencies(${PROJECT_BINARY_NAME} ${PROJECT_BINARY_NAME}_diffuser)
        if (NOT SDGUI_LLAMA_DISABLE)
            add_subdirectory(llama)
            add_dependencies(${PROJECT_BINARY_NAME} ${PROJECT_BINARY_NAME}_llama)
        endif()
    endif()

    if (NOT SDGUI_WINDOWLESS OR SDGUI_SERVER)
        message(STATUS "Socket server: ${SDGUI_SERVER}")
        if (SDGUI_SERVER)
            add_subdirectory(server)
            add_dependencies(${PROJECT_BINARY_NAME}_server ${PROJECT_BINARY_NAME}_llama)
            add_dependencies(${PROJECT_BINARY_NAME}_server ${PROJECT_BINARY_NAME}_diffuser)
        endif()
    endif()
endif()



include(cmake/intl.cmake)
include(cmake/cpack.cmake)

# show a summary of the build configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

message(STATUS "Components: ")
foreach(item IN LISTS CPACK_COMPONENTS_ALL)
    message("\t\t * ${item}")
endforeach()

if (PO_FILES)
    message(STATUS "Translations: ")
    foreach(item IN LISTS PO_FILES)
        message("\t\t * ${item}")
    endforeach()
endif()