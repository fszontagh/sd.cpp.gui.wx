if (NOT WIN32)

    set(wxWidgets_USE_STATIC ON)
    set(wxBUILD_SHARED OFF)
    set(WXWIDGETS_EXTRA_PATH "" CACHE STRING "wxWidget extra search path, default: \"\"")

    

    set(wxWidgets_TYPE "system")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_check_modules(PANGO REQUIRED pango)
    
    if (NOT WXWIDGETS_EXTRA_PATH STREQUAL "")
        
        message(STATUS "WXWIDGETS_EXTRA_PATH: ${WXWIDGETS_EXTRA_PATH}")

        find_package(wxWidgets ${WXWIDGETS_VERSION} QUIET
        PATHS ${WXWIDGETS_EXTRA_PATH})

        else()

        find_package(wxWidgets ${WXWIDGETS_VERSION} QUIET)

    endif()
    
    
    
    
    if (NOT wxWidgets_FOUND)

        message(STATUS "wxWidgets not found, using fetch content with tag v${WXWIDGETS_VERSION}")
        set(wxWidgets_TYPE "download")
    
        FetchContent_Declare(
            wxWidgets
            GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
            GIT_TAG "v${WXWIDGETS_VERSION}"
            EXCLUDE_FROM_ALL
            FIND_PACKAGE_ARGS NAMES wxWidgets
        )
        FetchContent_MakeAvailable(wxWidgets)

        if(NOT wxwidgets_POPULATED)
            FetchContent_Populate(wxwidgets)
            add_subdirectory(${wxwidgets_SOURCE_DIR} ${wxwidgets_BUILD_DIR} EXCLUDE_FROM_ALL)
        endif()
        set(wxWidgets_FOUND ON)
        set(wxWidgets_VERSION ${WXWIDGETS_VERSION})


    endif(NOT wxWidgets_FOUND)
else()
	find_package(wxWidgets)
endif(NOT WIN32)

if (NOT wxWidgets_FOUND)
    message(FATAL_ERROR "wxWidgets not found")
else()
    message(STATUS "wxWidgets found: ${wxWidgets_VERSION} ${wxWidgets_TYPE} ${wxWidgets_DIR}")
endif()
