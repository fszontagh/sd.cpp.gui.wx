# CPack configuration
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "StableDiffusionGUI")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "Ferenc Szonta'gh <szf@fsociety.hu>")

set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN TRUE)
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED FALSE)


set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "applications")

# Set OS-specific settings
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME ${CMAKE_PROJECT_NAME})
    set(CPACK_NSIS_PACKAGE_NAME "${CMAKE_PROJECT_NAME} ${PROJECT_VERSION}")
    set(CPACK_NSIS_URL_INFO_ABOUT ${CMAKE_PROJECT_HOMEPAGE_URL})
    set(CPACK_NSIS_HELP_LINK "https://github.com/fszontagh/sd.cpp.gui.wx")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/graphics/icons/256/cube.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/graphics/icons/256/cube.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "StableDiffusionGUI.exe")
    
elseif(UNIX AND NOT APPLE)
    # Linux - AppImage and DEB packages
    set(CPACK_GENERATOR "DEB")
    
    # AppImage settings
    set(CPACK_APPIMAGE_FILE_NAME "StableDiffusionGUI-${PROJECT_VERSION}-${CMAKE_SYSTEM_PROCESSOR}.AppImage")
    set(CPACK_APPIMAGE_UPDATE_INFORMATION "gh-releases-zsync|fszontagh|StableDiffusionGUI|latest|StableDiffusionGUI-${PROJECT_VERSION}-${CMAKE_SYSTEM_PROCESSOR}.AppImage.zsync")

    # DEB package settings
    set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ferenc Szontágh <szf@fsociety.hu>")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.29), libstdc++6 (>= 9), libgtk-3-0 (>= 3.9.10) | libgtk-4-1, libudev1 (>= 183), libvulkan1, libx11-6")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CMAKE_PROJECT_DESCRIPTION})
    set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

    
    # Generate the .desktop file by replacing placeholders
    configure_file(${CMAKE_SOURCE_DIR}/platform/linux/app.desktop ${CMAKE_BINARY_DIR}/stablediffusiongui.desktop @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/stablediffusiongui.desktop DESTINATION share/applications COMPONENT applications)
    install(FILES ${CMAKE_SOURCE_DIR}/graphics/icons/256/stablediffusiongui.png DESTINATION share/icons/hicolor/256x256/apps COMPONENT applications)    




endif()

install(TARGETS sd_ui DESTINATION bin COMPONENT applications)

if (SDCPP_AVX_LIBRARY)
    install(FILES ${SDCPP_AVX_LIBRARY} DESTINATION lib COMPONENT applications)
endif()

if (SDCPP_AVX2_LIBRARY)
    install(FILES ${SDCPP_AVX2_LIBRARY} DESTINATION lib COMPONENT applications)
endif()

if (SDCPP_AVX512_LIBRARY)
    install(FILES ${SDCPP_AVX512_LIBRARY} DESTINATION lib COMPONENT applications)
endif()

if (SDCPP_CUDA_LIBRARY)
    install(FILES ${SDCPP_CUDA_LIBRARY} DESTINATION lib COMPONENT applications)
endif()

if (SDCPP_ROCM_LIBRARY)
    install(FILES ${SDCPP_ROCM_LIBRARY} DESTINATION lib COMPONENT applications)
endif()

#get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
list(REMOVE_ITEM CPACK_COMPONENTS_ALL "Unspecified")
set(CPACK_COMPONENTS_ALL applications)
message("CPACK_COMPONENTS_ALL: ${CPACK_COMPONENTS_ALL}")

# CPack settings
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
#set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/WELCOME.txt")

# Ensure the version numbers
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

get_cmake_property(LIST_OF_COMPONENTS COMPONENTS)
message("DEBUG: Component List: ${LIST_OF_COMPONENTS}")


include(CPack)
