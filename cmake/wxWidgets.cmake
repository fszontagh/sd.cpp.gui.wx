if (NOT WIN32)

    set(wxWidgets_USE_STATIC ON)
    set(wxBUILD_SHARED OFF)


    FetchContent_Declare(
        wxWidgets
        GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
        GIT_TAG v3.2.6
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(wxWidgets)

    if(NOT wxwidgets_POPULATED)
        FetchContent_Populate(wxwidgets)
        add_subdirectory(${wxwidgets_SOURCE_DIR} ${wxwidgets_BUILD_DIR} EXCLUDE_FROM_ALL)
    endif()
else()
	find_package(wxWidgets)
endif(NOT WIN32)
