
set(APP_IMAGE_ICON_FILE ${CMAKE_SOURCE_DIR}/graphics/icons/256/stablediffusiongui.png)
set(APP_IMAGE_DESKTOP_FILE ${CMAKE_BINARY_DIR}/stablediffusiongui.desktop)

configure_file(${CMAKE_SOURCE_DIR}/platform/linux/AppImageRecipe.yml.in ${CMAKE_BINARY_DIR}/AppImageRecipe.yml @ONLY)


set(APPIMGDEPENDS ${PROJECT_BINARY_NAME})
if (SDGUI_LOCAL_DISABLE EQUAL OFF)
  list(APPEND APPIMGDEPENDS ${PROJECT_BINARY_NAME}_diffuser)
  if (SDGUI_LLAMA_DISABLE EQUAL OFF)
    list(APPEND APPIMGDEPENDS ${PROJECT_BINARY_NAME}_llama)
  endif()
endif()

if (SDGUI_AVX AND SDGUI_LOCAL_DISABLE EQUAL OFF)
  list(APPEND APPIMGDEPENDS stable_diffusion_cpp_avx)
  if (SDGUI_LLAMA_DISABLE EQUAL OFF)
    list(APPEND APPIMGDEPENDS llama_cpp_avx)
  endif()
endif()

if (SDGUI_AVX2 AND SDGUI_LOCAL_DISABLE EQUAL OFF)
  list(APPEND APPIMGDEPENDS stable_diffusion_cpp_avx2)
  if (SDGUI_LLAMA_DISABLE EQUAL OFF)
    list(APPEND APPIMGDEPENDS llama_cpp_avx2)
  endif()
endif()

if(SDGUI_AVX512 AND SDGUI_LOCAL_DISABLE EQUAL OFF)
  list(APPEND APPIMGDEPENDS stable_diffusion_cpp_avx512)
  if (SDGUI_LLAMA_DISABLE EQUAL OFF)
    list(APPEND APPIMGDEPENDS llama_cpp_avx512)
  endif()
endif()

if(SDGUI_CUDA AND SDGUI_LOCAL_DISABLE EQUAL OFF)
  list(APPEND APPIMGDEPENDS stable_diffusion_cpp_cuda)
  if (SDGUI_LLAMA_DISABLE EQUAL OFF)
    list(APPEND APPIMGDEPENDS llama_cpp_cuda)
  endif()
endif()

if(SDGUI_VULKAN AND SDGUI_LOCAL_DISABLE EQUAL OFF)
  list(APPEND APPIMGDEPENDS stable_diffusion_cpp_vulkan)
  if (SDGUI_LLAMA_DISABLE EQUAL OFF)
    list(APPEND APPIMGDEPENDS llama_cpp_vulkan)
  endif()
endif()

if (SDGUI_HIPBLAS AND SDGUI_LOCAL_DISABLE EQUAL OFF)
  list(APPEND APPIMGDEPENDS stable_diffusion_cpp_hipblas)
  if (SDGUI_LLAMA_DISABLE EQUAL OFF)
    list(APPEND APPIMGDEPENDS llama_cpp_hipblas)
  endif()
endif()
add_custom_target(
  AppImage
  DEPENDS ${APPIMGDEPENDS}
)


add_custom_command(
  TARGET AppImage POST_BUILD
  MAIN_DEPENDENCY ${PROJECT_BINARY_NAME}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix ${CMAKE_BINARY_DIR}/AppImageSource/usr --config ${CMAKE_BUILD_TYPE}
  COMMAND wget -q https://github.com/AppImageCommunity/pkg2appimage/releases/download/continuous/pkg2appimage--x86_64.AppImage -O ./pkg2appimage
  COMMAND chmod +x ./pkg2appimage
  COMMAND sh -c "./pkg2appimage --appimage-extract > /dev/null 2>&1"
  COMMAND sh -c "./squashfs-root/AppRun AppImageRecipe.yml > /dev/null 2>&1"
  COMMAND ls -ltrh out
  COMMENT "Creating AppImage source directory"
  VERBATIM)
