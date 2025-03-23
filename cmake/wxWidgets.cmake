if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /OPT:REF /OPT:ICF")
elseif (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
endif()

if (NOT WIN32)
    set(wxWidgets_USE_STATIC ON)
    set(wxBUILD_SHARED OFF)
    set(wxBUILD_SAMPLES OFF)
    set(wxBUILD_TESTS OFF)
    set(wxBUILD_DEMOS OFF)
    set(wxBUILD_BENCHMARKS OFF)

    set(WXWIDGETS_EXTRA_PATH "" CACHE STRING "wxWidget extra search path, default: \"\"")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_check_modules(PANGO REQUIRED pango)

    if (NOT WXWIDGETS_EXTRA_PATH STREQUAL "")
        message(STATUS "Using extra search path: ${WXWIDGETS_EXTRA_PATH}")
        find_package(wxWidgets ${WXWIDGETS_VERSION} QUIET PATHS ${WXWIDGETS_EXTRA_PATH})
    else()
        find_package(wxWidgets ${WXWIDGETS_VERSION} QUIET)
    endif()


    if (wxWidgets_FOUND)
        set(wxWidgets_TYPE "system")
        message(STATUS "wxWidgets found on system: ${wxWidgets_VERSION} at ${wxWidgets_DIR}")
    else()
        message(STATUS "wxWidgets not found on system, using FetchContent with tag v${WXWIDGETS_VERSION}")
        set(wxWidgets_TYPE "download")

        include(FetchContent)
        FetchContent_Declare(
            wxWidgets
            GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
            GIT_TAG v${WXWIDGETS_VERSION}
        )
        FetchContent_MakeAvailable(wxWidgets)

        if (NOT wxwidgets_POPULATED)
            FetchContent_Populate(wxwidgets)
            add_subdirectory(${wxwidgets_SOURCE_DIR} ${wxwidgets_BUILD_DIR} EXCLUDE_FROM_ALL)
        endif()

        set(wxWidgets_FOUND ON)
        set(wxWidgets_VERSION ${WXWIDGETS_VERSION})
    endif()
else(IF NOT WIN32)
    find_package(wxWidgets REQUIRED)
    set(wxWidgets_TYPE "system")
endif()

if (NOT wxWidgets_FOUND)
    message(FATAL_ERROR "wxWidgets not found")
endif()

