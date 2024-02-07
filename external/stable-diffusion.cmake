
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug)
endif()

include(ExternalProject)

ExternalProject_Add(stable-diffusion
    GIT_REPOSITORY https://github.com/fszontagh/stable-diffusion.cpp.git
    PREFIX ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion
    DOWNLOAD_DIR ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion
    BINARY_DIR ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}/stable-diffusion -DSD_CUBLAS=${SD_CUBLAS} -DSD_HIPBLAS=${SD_HIPBLAS} -DSD_METAL=${SD_METAL} -DSD_BUILD_EXAMPLES=OFF -DSD_FLASH_ATTN=${SD_FLASH_ATTN} -DBUILD_SHARED_LIBS=ON
)



set(stable-diffusion_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/external/stable-diffusion/src/stable-diffusion")
set(stable-diffusion_LIBRARIES "${CMAKE_SOURCE_DIR}/external/stable-diffusion/${CMAKE_BUILD_TYPE}")
include_directories(${stable-diffusion_INCLUDE_DIRS})


if(WIN32)
add_library(ggmllib SHARED IMPORTED)
add_library(stable-diffusionlib SHARED IMPORTED)

set(GGML_LIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/ggml/src/${CMAKE_BUILD_TYPE}/ggml.lib)
set(GGML_SLIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/${CMAKE_BUILD_TYPE}/ggml.dll)

set(SD_LIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/${CMAKE_BUILD_TYPE}/stable-diffusion.lib)
set(SD_SLIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/${CMAKE_BUILD_TYPE}/stable-diffusion.dll)

set_target_properties(stable-diffusionlib PROPERTIES
IMPORTED_CONFIGURATIONS        "DEBUG;RELEASE"
IMPORTED_LOCATION_DEBUG        ${SD_SLIB}
IMPORTED_IMPLIB_DEBUG          ${SD_LIB}
IMPORTED_LOCATION_RELEASE      ${SD_SLIB}
IMPORTED_IMPLIB_RELEASE        ${SD_LIB}
INTERFACE_INCLUDE_DIRECTORIES  ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/src/stable-diffusion
)

set_target_properties(ggmllib PROPERTIES
IMPORTED_CONFIGURATIONS        "DEBUG;RELEASE"
IMPORTED_LOCATION_DEBUG        ${GGML_SLIB}
IMPORTED_IMPLIB_DEBUG          ${GGML_LIB}
IMPORTED_LOCATION_RELEASE      ${GGML_SLIB}
IMPORTED_IMPLIB_RELEASE        ${GGML_LIB}
)


else()

add_library(ggmllib SHARED IMPORTED)
add_library(stable-diffusionlib SHARED IMPORTED)

set(GGML_LIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/ggml/src/${CMAKE_BUILD_TYPE}/ggml.a)
set(GGML_SLIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/${CMAKE_BUILD_TYPE}/ggml.so)

set(SD_LIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/${CMAKE_BUILD_TYPE}/stable-diffusion.a)
set(SD_SLIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/${CMAKE_BUILD_TYPE}/stable-diffusion.so)

set_target_properties(stable-diffusionlib PROPERTIES
IMPORTED_CONFIGURATIONS        "DEBUG;RELEASE"
IMPORTED_LOCATION_DEBUG        ${SD_SLIB}
IMPORTED_IMPLIB_DEBUG          ${SD_LIB}
IMPORTED_LOCATION_RELEASE      ${SD_SLIB}
IMPORTED_IMPLIB_RELEASE        ${SD_LIB}
INTERFACE_INCLUDE_DIRECTORIES  ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/src/stable-diffusion
)

set_target_properties(ggmllib PROPERTIES
IMPORTED_CONFIGURATIONS        "DEBUG;RELEASE"
IMPORTED_LOCATION_DEBUG        ${GGML_SLIB}
IMPORTED_IMPLIB_DEBUG          ${GGML_LIB}
IMPORTED_LOCATION_RELEASE      ${GGML_SLIB}
IMPORTED_IMPLIB_RELEASE        ${GGML_LIB}
)






endif()