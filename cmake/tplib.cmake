    FetchContent_Declare(
        tpl
        GIT_REPOSITORY https://gitlab.com/eidheim/tiny-process-library.git
        GIT_TAG v2.0.4
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(tpl)

    if(NOT tpl_POPULATED)
        FetchContent_Populate(tpl)
        add_subdirectory(${tpl_SOURCE_DIR} EXCLUDE_FROM_ALL)
    endif()
