if (WIN32)
    ExternalProject_Add(curl
    GIT_REPOSITORY "https://github.com/curl/curl"
    GIT_TAG "tags/curl-8_8_0"
    GIT_CONFIG advice.detachedHead=false
    PREFIX "${CMAKE_BINARY_DIR}/curl"
    CMAKE_CACHE_ARGS
        "-DBUILD_SHARED_LIBS:BOOL=OFF"
        "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}"
    )

    ExternalProject_Get_Property(curl SOURCE_DIR)
    set(CURL_INCLUDE_DIR "${SOURCE_DIR}/include")
    set(CURL_LIBRARY "${SOURCE_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}curl${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()