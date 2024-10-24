file(GENERATE
  OUTPUT "${PROJECT_BINARY_DIR}/appimage-generate.cmake"
  CONTENT [[
include(CMakePrintHelpers)
cmake_print_variables(CPACK_TEMPORARY_DIRECTORY)
cmake_print_variables(CPACK_TOPLEVEL_DIRECTORY)
cmake_print_variables(CPACK_PACKAGE_DIRECTORY)
cmake_print_variables(CPACK_PACKAGE_FILE_NAME)

find_program(LINUXDEPLOY_EXECUTABLE
  NAMES linuxdeploy linuxdeploy-x86_64.AppImage
  PATHS ${CPACK_PACKAGE_DIRECTORY}/linuxdeploy)

if (NOT LINUXDEPLOY_EXECUTABLE)
  message(STATUS "Downloading linuxdeploy")
  set(LINUXDEPLOY_EXECUTABLE ${CPACK_PACKAGE_DIRECTORY}/linuxdeploy/linuxdeploy)
  file(DOWNLOAD 
      https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
      ${LINUXDEPLOY_EXECUTABLE}
      INACTIVITY_TIMEOUT 10
      LOG ${CPACK_PACKAGE_DIRECTORY}/linuxdeploy/download.log
      STATUS LINUXDEPLOY_DOWNLOAD)
  execute_process(COMMAND chmod +x ${LINUXDEPLOY_EXECUTABLE} COMMAND_ECHO STDOUT)
endif()

execute_process(
  COMMAND
    ${CMAKE_COMMAND} -E env
      OUTPUT=${CPACK_PACKAGE_FILE_NAME}.appimage
      VERSION=$<IF:$<BOOL:${CPACK_PACKAGE_VERSION}>,${CPACK_PACKAGE_VERSION},0.1.0>
    ${LINUXDEPLOY_EXECUTABLE}
    --appdir=${CPACK_TEMPORARY_DIRECTORY}
    --executable=$<TARGET_FILE:sd_ui>
    $<$<BOOL:$<TARGET_PROPERTY:sd_ui,APPIMAGE_DESKTOP_FILE>>:--desktop-file=$<TARGET_PROPERTY:sd_ui,APPIMAGE_DESKTOP_FILE>>
    $<$<BOOL:$<TARGET_PROPERTY:sd_ui,APPIMAGE_ICON_FILE>>:--icon-file=$<TARGET_PROPERTY:sd_ui,APPIMAGE_ICON_FILE>>
    --output=appimage
    #    --verbosity=2
)
]])

list(APPEND CPACK_GENERATOR External)

set(CPACK_EXTERNAL_PACKAGE_SCRIPT "${PROJECT_BINARY_DIR}/appimage-generate.cmake")
set(CPACK_EXTERNAL_ENABLE_STAGING YES)
set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})