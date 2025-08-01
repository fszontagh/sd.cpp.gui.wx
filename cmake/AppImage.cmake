
set(APP_IMAGE_ICON_FILE ${CMAKE_SOURCE_DIR}/graphics/icons/256/stablediffusiongui.png)
set(APP_IMAGE_DESKTOP_FILE ${CMAKE_BINARY_DIR}/stablediffusiongui.desktop)

configure_file(${CMAKE_SOURCE_DIR}/platform/linux/AppImageRecipe.yml.in ${CMAKE_BINARY_DIR}/AppImageRecipe.yml @ONLY)


set(APPIMGDEPENDS ${PROJECT_BINARY_NAME})
list(APPEND APPIMGDEPENDS ${PROJECT_BINARY_NAME}_diffuser)

if (SDGUI_AVX)
  list(APPEND APPIMGDEPENDS stable-diffusion_avx)
endif()

if (SDGUI_AVX2)
  list(APPEND APPIMGDEPENDS stable-diffusion_avx2)
endif()

if(SDGUI_AVX512)
  list(APPEND APPIMGDEPENDS stable-diffusion_avx512)
endif()

if(SDGUI_CUBLAS)
  list(APPEND APPIMGDEPENDS stable-diffusion_cuda)
endif()

if(SDGUI_VULKAN)
  list(APPEND APPIMGDEPENDS stable-diffusion_vulkan)
endif()

if (SDGUI_HIPBLAS)
  list(APPEND APPIMGDEPENDS stable-diffusion_hipblas)
endif()




message(STATUS "APPIMGDEPENDS: ${APPIMGDEPENDS}")

add_custom_target(
  AppImage
  DEPENDS ${APPIMGDEPENDS}
)


add_custom_command(
  TARGET AppImage POST_BUILD
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix ${CMAKE_BINARY_DIR}/AppImageSource/usr --config ${CMAKE_BUILD_TYPE}
  COMMAND wget -q https://github.com/AppImageCommunity/pkg2appimage/releases/download/continuous/pkg2appimage--x86_64.AppImage -O ./pkg2appimage
  COMMAND chmod +x ./pkg2appimage
  COMMAND sh -c "./pkg2appimage --appimage-extract > /dev/null 2>&1"
  COMMAND sh -c "./squashfs-root/AppRun AppImageRecipe.yml > /dev/null 2>&1"
  COMMAND ls -ltrh out
  COMMENT "Creating AppImage source directory"
  VERBATIM)
