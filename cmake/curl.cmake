#if (WIN32)

set(BUILD_CURL_EXE OFF)
set(BUILD_CURL_TESTS OFF)
set(BUILD_CURL_SHARED OFF)
set(BUILD_CURL_STATIC ON)
set(HTTP_ONLY ON)

set(BUILD_LIBCURL_DOCS OFF)
set(BUILD_MISC_DOCS OFF)
set(ENABLE_CURL_MANUAL OFF)


FetchContent_Declare(
    libcurl
    GIT_REPOSITORY https://github.com/curl/curl
    GIT_TAG curl-8_10_1
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(libcurl)

if(NOT libcurl_POPULATED)
    FetchContent_Populate(libcurl)
    add_subdirectory(${cURL_SOURCE_DIR} ${cURL_BUILD_DIR} EXCLUDE_FROM_ALL)
endif()
