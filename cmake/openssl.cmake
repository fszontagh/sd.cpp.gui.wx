#https://github.com/viaduck/openssl-cmake.git

ExternalProject_Add(
    opensslcmake
    GIT_REPOSITORY https://github.com/viaduck/openssl-cmake.git
    GIT_TAG "master"
    INSTALL_COMMAND ""
    SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/stable_diffusion_cpp_avx
    )
    FetchContent_MakeAvailable(opensslcmake)

    if(NOT opensslcmake_POPULATED)
        FetchContent_Populate(opensslcmake)
        add_subdirectory(${opensslcmake_SOURCE_DIR} ${opensslcmake_BUILD_DIR} EXCLUDE_FROM_ALL)
    endif()
