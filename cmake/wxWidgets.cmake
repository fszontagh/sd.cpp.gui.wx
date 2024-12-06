if (NOT WIN32)

    set(wxWidgets_USE_STATIC ON)
    set(wxBUILD_SHARED OFF)
    option(wxWidgets_EXTRA_PATH "wxWidget extra search path" OFF)
    
    set(wxWidgets_TYPE "system")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_check_modules(PANGO REQUIRED pango)
    
    find_package(wxWidgets ${WXWIDGETS_VERSION} QUIET 
     PATHS ${wxWidgets_EXTRA_PATH}
    )
    

    if (NOT wxWidgets_FOUND)

        message(STATUS "wxWidgets not found, downloading...")
        set(wxWidgets_TYPE "download")
    
        FetchContent_Declare(
            wxWidgets
            GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
            GIT_TAG "v${WXWIDGETS_VERSION}"
            EXCLUDE_FROM_ALL
        )
        FetchContent_MakeAvailable(wxWidgets)

        if(NOT wxwidgets_POPULATED)
            FetchContent_Populate(wxwidgets)
            add_subdirectory(${wxwidgets_SOURCE_DIR} ${wxwidgets_BUILD_DIR} EXCLUDE_FROM_ALL)
        endif()

    endif(NOT wxWidgets_FOUND)
else()
	find_package(wxWidgets)
endif(NOT WIN32)

if (NOT wxWidgets_FOUND)
    message(FATAL_ERROR "wxWidgets not found")
    else()
        message(STATUS "wxWidgets found: ${wxWidgets_VERSION} ${wxWidgets_TYPE}")
endif(NOT wxWidgets_FOUND)
