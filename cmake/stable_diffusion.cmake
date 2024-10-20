
include(ExternalProject)

set(SD_BUILD_EXAMPLES OFF)
set(SD_BUILD_SHARED_LIBS ON)

set(stable_diffusion_install_dir ${CMAKE_BINARY_DIR}/stable_diffusion_cpp)
#set(stable_diffurions_library ${CMAKE_BINARY_DIR}/exiv2-install/lib/${CMAKE_FIND_LIBRARY_PREFIXES}exiv2${CMAKE_SHARED_LIBRARY_SUFFIX})

set(PATCH_COMMAND "")

if (WIN32)
    set(PATCH_COMMAND ${CMAKE_SOURCE_DIR}/patches/apply.bat)
else()
    set(PATCH_COMMAND ${CMAKE_SOURCE_DIR}/patches/apply.sh)
endif()

set(SD_GIT_TAG "tags/master-e410aeb")

if (SD_AVX)


    set(SDCPP_AVX_LIBRARY ${CMAKE_BINARY_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}stable-diffusion_avx${CMAKE_SHARED_LIBRARY_SUFFIX})

    ExternalProject_Add(
    stable_diffusion_cpp_avx
    GIT_REPOSITORY https://github.com/leejet/stable-diffusion.cpp.git
    GIT_TAG ${SD_GIT_TAG}
    CMAKE_ARGS -DSD_CUBLAS=OFF -DGGML_CUBLAS=OFF -DGGML_AVX512=OFF -DGGML_AVX2=OFF -DGGML_AVX=ON -DSD_BUILD_EXAMPLES=OFF -DSD_BUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=${stable_diffusion_install_dir}_avx
    INSTALL_COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${CMAKE_BINARY_DIR}/sdcpp_avx/bin/${CMAKE_FIND_LIBRARY_PREFIXES}stable-diffusion${CMAKE_SHARED_LIBRARY_SUFFIX} ${SDCPP_AVX_LIBRARY}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/stable_diffusion_cpp_avx
    BINARY_DIR ${CMAKE_BINARY_DIR}/sdcpp_avx
    PATCH_COMMAND ${CMAKE_COMMAND} -E env bash ${PATCH_COMMAND} "${CMAKE_BINARY_DIR}/_deps/stable_diffusion_cpp_avx"
    )

    message(STATUS "SD_AVX: ${SDCPP_AVX_LIBRARY}")

    
endif()

if (SD_AVX2)

    set(SDCPP_AVX2_LIBRARY ${CMAKE_BINARY_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}stable-diffusion_avx2${CMAKE_SHARED_LIBRARY_SUFFIX})

    ExternalProject_Add(
    stable_diffusion_cpp_avx2
    GIT_REPOSITORY https://github.com/leejet/stable-diffusion.cpp.git
    GIT_TAG ${SD_GIT_TAG}
    CMAKE_ARGS -DSD_CUBLAS=OFF -DGGML_CUBLAS=OFF -DGGML_AVX512=OFF -DGGML_AVX2=ON -DGGML_AVX=OFF -DSD_BUILD_EXAMPLES=OFF -DSD_BUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=${stable_diffusion_install_dir}_avx2
    INSTALL_COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${CMAKE_BINARY_DIR}/sdcpp_avx2/bin/${CMAKE_FIND_LIBRARY_PREFIXES}stable-diffusion${CMAKE_SHARED_LIBRARY_SUFFIX} ${SDCPP_AVX2_LIBRARY}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/stable_diffusion_cpp_avx2
    BINARY_DIR ${CMAKE_BINARY_DIR}/sdcpp_avx2
    PATCH_COMMAND ${CMAKE_COMMAND} -E env bash ${PATCH_COMMAND} "${CMAKE_BINARY_DIR}/_deps/stable_diffusion_cpp_avx2"
    )   

    message(STATUS "SD_AVX2: ${SDCPP_AVX2_LIBRARY}")

endif()

if (SD_AVX512)

    set(SDCPP_AVX512_LIBRARY ${CMAKE_BINARY_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}stable-diffusion_avx512${CMAKE_SHARED_LIBRARY_SUFFIX})

    ExternalProject_Add(
    stable_diffusion_cpp_avx512
    GIT_REPOSITORY https://github.com/leejet/stable-diffusion.cpp.git
    GIT_TAG ${SD_GIT_TAG}
    CMAKE_ARGS -DSD_CUBLAS=OFF -DGGML_CUBLAS=OFF -DGGML_AVX512=ON -DGGML_AVX2=OFF -DGGML_AVX=OFF -DSD_BUILD_EXAMPLES=OFF -DSD_BUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=${stable_diffusion_install_dir}_avx512
    INSTALL_COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${CMAKE_BINARY_DIR}/sdcpp_avx512/bin/${CMAKE_FIND_LIBRARY_PREFIXES}stable-diffusion${CMAKE_SHARED_LIBRARY_SUFFIX} ${SDCPP_AVX512_LIBRARY}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/stable_diffusion_cpp_avx512
    BINARY_DIR ${CMAKE_BINARY_DIR}/sdcpp_avx512
    PATCH_COMMAND ${CMAKE_COMMAND} -E env bash ${PATCH_COMMAND} "${CMAKE_BINARY_DIR}/_deps/stable_diffusion_cpp_avx512"
    )
    
    

    
    message(STATUS "SD_AVX512: ${SDCPP_AVX512_LIBRARY}")

endif()

if (SD_CUBLAS)

    set(SDCPP_CUDA_LIBRARY ${CMAKE_BINARY_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}stable-diffusion_cuda${CMAKE_SHARED_LIBRARY_SUFFIX})

    ExternalProject_Add(
    stable_diffusion_cpp_cuda
    GIT_REPOSITORY https://github.com/leejet/stable-diffusion.cpp.git
    GIT_TAG ${SD_GIT_TAG}
    CMAKE_ARGS -DSD_CUBLAS=ON -DGGML_CUBLAS=ON -DGGML_AVX512=OFF -DGGML_AVX2=OFF -DGGML_AVX=OFF -DSD_BUILD_EXAMPLES=OFF -DSD_BUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=${stable_diffusion_install_dir}_cuda
    INSTALL_COMMAND ${CMAKE_COMMAND} ARGS -E copy ${CMAKE_BINARY_DIR}/sdcpp_cuda/bin/${CMAKE_FIND_LIBRARY_PREFIXES}stable-diffusion${CMAKE_SHARED_LIBRARY_SUFFIX} ${SDCPP_CUDA_LIBRARY}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/stable_diffusion_cpp_cuda
    BINARY_DIR ${CMAKE_BINARY_DIR}/sdcpp_cuda
    PATCH_COMMAND ${CMAKE_COMMAND} -E env bash ${PATCH_COMMAND} "${CMAKE_BINARY_DIR}/_deps/stable_diffusion_cpp_cuda"
    )

    message(STATUS "SD_CUBLAS: ${SDCPP_CUDA_LIBRARY}")

endif()

if (SD_HIPBLAS)
    ExternalProject_Add(
    stable_diffusion_cpp_ROCM
    GIT_REPOSITORY https://github.com/leejet/stable-diffusion.cpp.git
    GIT_TAG ${SD_GIT_TAG}
    CMAKE_ARGS -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DSD_HIPBLAS=ON -DSD_CUBLAS=OFF -DGGML_CUBLAS=OFF -DGGML_AVX512=OFF -DGGML_AVX2=OFF -DGGML_AVX=OFF -DSD_BUILD_EXAMPLES=OFF -DSD_BUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=${stable_diffusion_install_dir}_cuda
    INSTALL_COMMAND ""
    SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/stable_diffusion_cpp_rocm
    BINARY_DIR ${CMAKE_BINARY_DIR}/sdcpp_rocm
    PATCH_COMMAND ${CMAKE_COMMAND} -E env bash ${PATCH_COMMAND} "${CMAKE_BINARY_DIR}/_deps/stable_diffusion_cpp_rocm"
    )

    set(SDCPP_ROCM_LIBRARY ${CMAKE_BINARY_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}stable-diffusion_rocm${CMAKE_SHARED_LIBRARY_SUFFIX})
    add_custom_command(
        OUTPUT ${SDCPP_ROCM_LIBRARY}
        DEPENDS ${CMAKE_BINARY_DIR}/sdcpp_rocm/bin/${CMAKE_FIND_LIBRARY_PREFIXES}stable-diffusion${CMAKE_SHARED_LIBRARY_SUFFIX}
        COMMENT "Copy ROCM library to ${SDCPP_ROCM_LIBRARY}"
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_if_different ${CMAKE_BINARY_DIR}/sdcpp_rocm/bin/${CMAKE_FIND_LIBRARY_PREFIXES}stable-diffusion${CMAKE_SHARED_LIBRARY_SUFFIX} ${SDCPP_ROCM_LIBRARY}
    )   

    message(STATUS ${SDCPP_ROCM_LIBRARY})

endif()