set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0079 NEW)

set(SD_BUILD_EXAMPLES OFF)
set(SD_BUILD_SHARED_LIBS ON)
set(ALLOW_DUPLICATE_CUSTOM_TARGETS ON)

set(SD_GIT_TAG "tags/master-e410aeb")


set(SDGUI_AVX OFF)
set(SDGUI_AVX2 OFF)
set(SDGUI_AVX512 OFF)
set(SDGUI_CUBLAS OFF)
set(SDGUI_ROCM OFF)
set(SDGUI_VULKAN OFF)

if (SD_AVX) 
    set(SDGUI_AVX ON)
endif()

if(SD_AVX2)
    set(SDGUI_AVX2 ON)
endif(SD_AVX2)

if(SD_AVX512)
    set(SDGUI_AVX512 ON)
endif(SD_AVX512)

if(SD_CUBLAS)
    set(SDGUI_CUBLAS ON)
endif(SD_CUBLAS)

if (SD_ROCM)
    set(SDGUI_ROCM ON)
endif(SD_ROCM)

if (SD_VULKAN)
    set(SDGUI_VULKAN ON)
endif(SD_VULKAN)

# Helper macro to build stable diffusion with different settings
macro(build_stable_diffusion variant_name avx_flag avx2_flag avx512_flag cublas_flag rocm_flag vulkan_flag)


    # Set flags for the variant
    set(SD_AVX "${avx_flag}")
    set(SD_AVX2 "${avx2_flag}")
    set(SD_AVX512 "${avx512_flag}")
    set(SD_CUBLAS "${cublas_flag}")
    set(SD_ROCM "${rocm_flag}")
    set(SD_VULKAN "${vulkan_flag}")

    set(EPREFIX "")

    if (WIN32)
        set(EPREFIX "${CMAKE_BUILD_TYPE}/")
    endif()    

    message(STATUS "Building sd.cpp variant: ${variant_name} with flags: ${SD_AVX} ${SD_AVX2} ${SD_AVX512} ${SD_CUBLAS} ${SD_ROCM} ${SD_VULKAN} EPREFIX: ${EPREFIX}")
    
   

    ExternalProject_Add(
        stable_diffusion_cpp_${variant_name}
        GIT_REPOSITORY https://github.com/leejet/stable-diffusion.cpp.git
        GIT_TAG ${SD_GIT_TAG}
        EXCLUDE_FROM_ALL
        BINARY_DIR ${CMAKE_BINARY_DIR}/sdcpp_${variant_name}
	    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DSD_BUILD_EXAMPLES=OFF -DSD_BUILD_SHARED_LIBS=ON -DGGML_AVX=${SD_AVX} -DGGML_AVX2=${SD_AVX2} -DGGML_AVX512=${SD_AVX512} -DSD_CUBLAS=${SD_CUBLAS} -DSD_ROCM=${SD_ROCM} -DSD_VULKAN=${SD_VULKAN}
        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/sdcpp_${variant_name}/bin/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_${variant_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
    )

    
install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_${variant_name}${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION lib COMPONENT "SD.CPP - ${variant_name}")


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


if (SDGUI_ROCM)
build_stable_diffusion("rocm" OFF OFF OFF OFF ON OFF)
endif(SDGUI_ROCM)

if (SDGUI_VULKAN)
build_stable_diffusion("vulkan" OFF OFF OFF OFF OFF ON)
endif(SDGUI_VULKAN)
