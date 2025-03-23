set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0079 NEW)

set(SD_BUILD_EXAMPLES OFF)
set(SD_BUILD_SHARED_LIBS ON)
set(ALLOW_DUPLICATE_CUSTOM_TARGETS ON)




macro(build_llama variant_name avx_flag avx2_flag avx512_flag cuda_flag hipblas_flag vulkan_flag)


    set(LOG_DIR ${CMAKE_BINARY_DIR}/llama_${variant_name})

    set(SD_AVX "${avx_flag}")
    set(SD_AVX2 "${avx2_flag}")
    set(SD_AVX512 "${avx512_flag}")
    set(SD_CUDA "${cuda_flag}")
    set(SD_HIPBLAS "${hipblas_flag}")
    set(SD_VULKAN "${vulkan_flag}")


    set(GGML_CPU OFF)

    if (SD_AVX OR SD_AVX2 OR SD_AVX512)
        set(GGML_CPU ON)
    endif()

    if (MSVC)
        set(DISABLE_WARNINGS_FLAGS "/W0")
    else()
        set(DISABLE_WARNINGS_FLAGS "-w -Wno-deprecated")
    endif()

    set(EPREFIX "")

    if (WIN32)
        set(EPREFIX "${CMAKE_BUILD_TYPE}/")
    endif()

    if (CMAKE_GENERATOR MATCHES "Ninja")
        set(EPREFIX "")
    endif()

    message(STATUS "Building llama.cpp variant: ${variant_name} with flags: ${SD_AVX} ${SD_AVX2} ${SD_AVX512} ${SD_CUDA} ${SD_HIPBLAS} ${SD_VULKAN} EPREFIX: ${EPREFIX} Generator: ${CMAKE_GENERATOR}")
    if (SD_CUDA)

        #enable_language(CUDA)
        find_program(NVCC nvcc
        HINTS
            "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/bin"
    )

    if (NOT NVCC)
        message(FATAL_ERROR "CUDA compiler (nvcc) not found!")
    endif()

    execute_process(
        COMMAND ${NVCC} --version
        OUTPUT_VARIABLE CUDA_COMPILER_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

        string(REGEX MATCH "release ([0-9]+)\\.([0-9]+), V([0-9]+)\\.([0-9]+)\\.([0-9]+)" CUDA_VERSION_MATCH "${CUDA_COMPILER_OUTPUT}")

        set(CUDA_MAJOR_VERSION "${CMAKE_MATCH_1}")
        set(CUDA_MINOR_VERSION "${CMAKE_MATCH_2}")
        set(CUDA_PATCH_VERSION "${CMAKE_MATCH_5}")

        message(STATUS "Detected CUDA Version: ${CUDA_MAJOR_VERSION}.${CUDA_MINOR_VERSION}.${CUDA_PATCH_VERSION}")

        if (CMAKE_BUILD_TYPE STREQUAL "Debug")

            SET(CMAKE_CUDA_ARCHITECTURES "native")
            SET(GGMAL_NATIVE ON)
            SET(GGML_CACHE OFF)

        else()
        SET(GGMAL_NATIVE OFF)
        SET(GGML_CACHE OFF)
            if (CUDA_MAJOR_VERSION STREQUAL "12")
                SET(CMAKE_CUDA_ARCHITECTURES "90;89;80;75")
            elseif (CUDA_MAJOR_VERSION STREQUAL "11" AND CUDA_MINOR_VERSION STREQUAL "5")
                SET(CMAKE_CUDA_ARCHITECTURES "52;61;70;75")
            endif()

        endif()
        string(REPLACE ";" "|" CMAKE_CUDA_ARCHITECTURES "${CMAKE_CUDA_ARCHITECTURES}")
        message(STATUS "CUDA_ARCHITECTURES: ${CMAKE_CUDA_ARCHITECTURES}")
    endif()

    set(LLAMA_INCLUDE_DIR ${CMAKE_BINARY_DIR}/llama_src_${variant_name}/include CACHE PATH "Path to llama.cpp include directory" FORCE)
    set(GGML_INCLUDE_DIR ${CMAKE_BINARY_DIR}/llama_src_${variant_name}/ggml/include CACHE PATH "Path to GGML include directory" FORCE)


    ExternalProject_Add(
        llama_cpp_${variant_name}
        GIT_REPOSITORY https://github.com/ggerganov/llama.cpp.git
        GIT_TAG ${LLAMA_VERSION}
        BINARY_DIR ${CMAKE_BINARY_DIR}/llama_${variant_name}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/llama_src_${variant_name}
        LIST_SEPARATOR "|"
        CMAKE_ARGS
            -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/llama_${variant_name}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_CXX_FLAGS=${DISABLE_WARNINGS_FLAGS}
            -DCMAKE_C_FLAGS=${DISABLE_WARNINGS_FLAGS}
            #-DLLAMA_BUILD_TESTS=OFF
            #-DLLAMA_BUILD_EXAMPLES=OFF
            #-DLLAMA_BUILD_SERVER=OFF
            #-DLLAMA_BUILD_COMMON=OFF
            #-DLLAMA_ALL_WARNINGS=OFF
            #-DLLAMA_ALL_WARNINGS_3RD_PARTY=OFF
            #-DLLAMA_CURL=OFF
            #-DLLAMA_LLGUIDANCE=OFF
            -DGGML_CPU_AARCH64=OFF
            -DGGML_AVX=${SD_AVX}
            -DGGML_AVX2=${SD_AVX2}
            -DGGML_AVX512=${SD_AVX512}
            -DGGML_CUDA=${SD_CUDA}
            -DGGML_VULKAN=${SD_VULKAN}
            -DGGML_ALL_WARNINGS=OFF
            -DGGML_NATIVE=${GGML_NATIVE}
            -DGGML_BACKEND_DL=OFF
            -DGGML_CCACHE=${GGML_CACHE}
            -DGGML_CPU=${GGML_CPU}
            -DCMAKE_CUDA_ARCHITECTURES=${CMAKE_CUDA_ARCHITECTURES}
            PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/cmake/llama_CMakeLists.txt ${CMAKE_BINARY_DIR}/llama_src_${variant_name}/CMakeLists.txt
            BUILD_COMMAND ${CMAKE_COMMAND} -E env PATH=${_BINPATH} ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/llama_${variant_name} --config ${CMAKE_BUILD_TYPE}
            INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/llama_${variant_name}/bin/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}llama${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}llama_${variant_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
            BYPRODUCTS ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}llama_${variant_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
            BUILD_ALWAYS OFF
    )

endmacro()

if (SDGUI_AVX)
    build_llama("avx" ON OFF OFF OFF OFF OFF)
endif()

if (SDGUI_AVX2)
    build_llama("avx2" OFF ON OFF OFF OFF OFF)
endif()

if (SDGUI_AVX512)
    build_llama("avx512" OFF OFF ON OFF OFF OFF)
endif()

if (SDGUI_CUDA)
    build_llama("cuda" OFF OFF OFF ON OFF OFF)
endif()

if (SDGUI_HIPBLAS)
    build_llama("hipblas" OFF OFF OFF OFF ON OFF)
endif()

if (SDGUI_VULKAN)
    build_llama("vulkan" OFF OFF OFF OFF OFF ON)
endif()

message(STATUS "LLAMA_INCLUDE_DIR: ${LLAMA_INCLUDE_DIR}")