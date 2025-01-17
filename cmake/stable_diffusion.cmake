set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0079 NEW)

set(SD_BUILD_EXAMPLES OFF)
set(SD_BUILD_SHARED_LIBS ON)
set(ALLOW_DUPLICATE_CUSTOM_TARGETS ON)



option(SD_AVX "Enable AVX library build" OFF)
option(SD_AVX2 "Enable AVX2 library build" OFF)
option(SD_AVX512 "Enable AVX512 library build" OFF)
option(SD_CUDA "Enable CUDA library build" OFF)
option(SD_HIPBLAS "Enable HIPBLAS library build" OFF)
option(SD_VULKAN "Enable Vulkan library build" OFF)

set(SD_GIT_TAG "tags/master-${SDCPP_VERSION}")

if (SD_AVX)
    set(SDGUI_AVX ON)
endif()


if (SD_AVX2)
    set(SDGUI_AVX2 ON)
endif()

if (SD_AVX512)
    set(SDGUI_AVX512 ON)
endif()


if (SD_CUDA)
    set(SDGUI_CUBLAS ON)
endif()


if (SD_HIPBLAS)
    set(SDGUI_HIPBLAS ON)
endif()


if (SD_VULKAN)
    set(SDGUI_VULKAN ON)
endif()

# Helper macro to build stable diffusion with different settings
macro(build_stable_diffusion variant_name avx_flag avx2_flag avx512_flag cublas_flag hipblas_flag vulkan_flag)
    set(LOG_DIR ${CMAKE_BINARY_DIR}/sdcpp_${variant_name})

    # Set flags for the variant
    set(SD_AVX "${avx_flag}")
    set(SD_AVX2 "${avx2_flag}")
    set(SD_AVX512 "${avx512_flag}")
    set(SD_CUDA "${cublas_flag}")
    set(SD_HIPBLAS "${hipblas_flag}")
    set(SD_VULKAN "${vulkan_flag}")

    set(EPREFIX "")

    if (WIN32)
        set(EPREFIX "${CMAKE_BUILD_TYPE}/")
    endif()

    if (CMAKE_GENERATOR MATCHES "Ninja")
        set(EPREFIX "")
    endif()

    message(STATUS "Building sd.cpp variant: ${variant_name} with flags: ${SD_AVX} ${SD_AVX2} ${SD_AVX512} ${SD_CUDA} ${SD_HIPBLAS} ${SD_VULKAN} EPREFIX: ${EPREFIX} Generator: ${CMAKE_GENERATOR}")


if (MSVC)
    set(DISABLE_WARNINGS_FLAGS "/W0")
else()
    set(DISABLE_WARNINGS_FLAGS "-w -Wno-deprecated")
endif()

if(NOT SD_HIPBLAS)



    file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/sdcpp_${variant_name}/bin/${EPREFIX}" _BINPATH)
    if (WIN32)
        list(APPEND ${_BINPATH} $ENV{PATH})
    else ()
        set(_BINPATH "${_BINPATH}:$ENV{PATH}")
    endif ()

    SET(GGML_NATIVE OFF)
    SET(GGML_CCACHE OFF)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        SET(GGML_NATIVE ON)
        SET(GGML_CCACHE ON)
    endif()
    set(STATUS "SD_GIT_TAG: ${SD_GIT_TAG}")
        ExternalProject_Add(
            stable_diffusion_cpp_${variant_name}
            GIT_REPOSITORY https://github.com/leejet/stable-diffusion.cpp.git
            GIT_TAG ${SD_GIT_TAG}
            #WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/sdcpp_${variant_name}
            BINARY_DIR ${CMAKE_BINARY_DIR}/sdcpp_${variant_name}
            CMAKE_ARGS "-DGGML_NATIVE=${GGML_NATIVE}"
            "-DGGML_CCACHE=${GGML_CCACHE}"
            "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}"
            "-DCMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION}"
            "-DCMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}"
            "-DCMAKE_CXX_FLAGS=${DISABLE_WARNINGS_FLAGS}"
            "-DCMAKE_C_FLAGS=${DISABLE_WARNINGS_FLAGS}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DSD_BUILD_EXAMPLES=OFF"
            "-DSD_BUILD_SHARED_LIBS=ON"
            "-DGGML_AVX=${SD_AVX}"
            "-DGGML_AVX2=${SD_AVX2}"
            "-DGGML_AVX512=${SD_AVX512}"
            "-DSD_CUDA=${SD_CUDA}"
            "-DSD_HIPBLAS=${SD_HIPBLAS}"
            "-DSD_VULKAN=${SD_VULKAN}"
            BUILD_COMMAND ${CMAKE_COMMAND} -E env PATH=${_BINPATH} ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/sdcpp_${variant_name} --config ${CMAKE_BUILD_TYPE}
            INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/sdcpp_${variant_name}/bin/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_${variant_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
            BYPRODUCTS ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_${variant_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
endif()

if(SD_HIPBLAS)

        ExternalProject_Add(
            stable_diffusion_cpp_${variant_name}
            GIT_REPOSITORY https://github.com/leejet/stable-diffusion.cpp.git
            CMAKE_GENERATOR Ninja
            GIT_TAG ${SD_GIT_TAG}
            LOG_OUTPUT_ON_FAILURE ON
            EXCLUDE_FROM_ALL
            BINARY_DIR ${CMAKE_BINARY_DIR}/sdcpp_${variant_name}
            CMAKE_ARGS -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME} -DCMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION} -DCMAKE_C_FLAGS=-Wno-nested-anon-types -DCMAKE_CXX_FLAGS=-Wno-nested-anon-types -G Ninja -DGPU_TARGETS=gfx1100,gfx1102,gfx1030 -DAMDGPU_TARGETS=gfx1100,gfx1102,gfx1030 -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DSD_BUILD_EXAMPLES=OFF -DSD_BUILD_SHARED_LIBS=ON -DGGML_NATIVE=OFF -DGGML_AVX=${SD_AVX} -DGGML_AVX2=${SD_AVX2} -DGGML_AVX512=${SD_AVX512} -DSD_CUDA=${SD_CUDA} -DSD_HIPBLAS=${SD_HIPBLAS} -DSD_VULKAN=${SD_VULKAN}
            INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/sdcpp_${variant_name}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_${variant_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
            INSTALL_BYPRODUCTS ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_${variant_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
        )


    endif(SD_HIPBLAS)

endmacro()



if (SDGUI_AVX)
    build_stable_diffusion("avx" ON OFF OFF OFF OFF OFF)
endif()

if (SDGUI_AVX2)
    build_stable_diffusion("avx2" OFF ON OFF OFF OFF OFF)
endif()

if (SDGUI_AVX512)
    build_stable_diffusion("avx512" OFF OFF ON OFF OFF OFF)
endif()

if(SDGUI_CUBLAS)
    build_stable_diffusion("cuda" OFF OFF OFF ON OFF OFF)
endif(SDGUI_CUBLAS)

if (SDGUI_HIPBLAS)
    build_stable_diffusion("hipblas" OFF OFF OFF OFF ON OFF)
endif(SDGUI_HIPBLAS)

if (SDGUI_VULKAN)
    build_stable_diffusion("vulkan" OFF OFF OFF OFF OFF ON)
endif(SDGUI_VULKAN)
