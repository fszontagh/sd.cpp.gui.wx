set(wxWidgets_USE_STATIC ON)
set(wxBUILD_SHARED OFF)

# Use wxWidgets with Qt instead of GTK
set(wxUSE_QT ON)
set(wxUSE_GUI ON)

FetchContent_Declare(
    wxWidgets
    GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
    GIT_TAG v3.2.6
)
FetchContent_MakeAvailable(wxWidgets)

if(NOT wxwidgets_POPULATED)
    FetchContent_Populate(wxwidgets)

    # Build wxWidgets with Qt support
    set(wxBUILD_QT ON CACHE BOOL "Enable Qt support" FORCE)
    add_subdirectory(${wxwidgets_SOURCE_DIR} ${wxwidgets_BUILD_DIR})
endif()