# CPack configuration

include(GNUInstallDirs)

set(CPACK_PACKAGE_NAME "StableDiffusionGUI")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "Ferenc Szonta'gh <szf@fsociety.hu>")
set(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}/graphics/icons/256/stablediffusiongui.png)
set(CPACK_PACKAGE_CHECKSUM "SHA256")
set(CPACK_PACKAGE_VENDOR "fszontagh")

set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN TRUE)
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED FALSE)




set(CPACK_COMPONENTS_ALL "${CMAKE_PROJECT_NAME}")


set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "${CMAKE_PROJECT_NAME}")

set(bin_INSTALL_PATH_NSIS "bin")
set(lib_INSTALL_PATH_NSIS "lib")

if (WIN32)
	set(bin_INSTALL_PATH_NSIS ".")
	set(lib_INSTALL_PATH_NSIS ".")
	set(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}\\\\graphics\\\\icons\\\\256\\\\stablediffusiongui.png)
endif()


if (UNIX AND NOT APPLE)
    execute_process(
        COMMAND lsb_release -si
        OUTPUT_VARIABLE DISTRO_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND lsb_release -sr
        OUTPUT_VARIABLE DISTRO_VER
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(DISTRO_VERSION "${DISTRO_NAME}${DISTRO_VER}")
endif()

# CPack settings
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")


# Ensure the version numbers
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_BINARY_NAME};Stable Diffusion GUI")
set(CPACK_STRIP_FILES TRUE)

#set_property(TARGET ${PROJECT_BINARY_NAME} PROPERTY VERSION "${PROJECT_VERSION}")
set_property(TARGET ${PROJECT_BINARY_NAME} PROPERTY SOVERSION "${GIT_HASH}")
set_property(TARGET ${PROJECT_BINARY_NAME} PROPERTY DESCRIPTION "${CMAKE_PROJECT_DESCRIPTION}")
set_property(TARGET ${PROJECT_BINARY_NAME} PROPERTY URL "${CMAKE_PROJECT_HOMEPAGE_URL}")
set_property(TARGET ${PROJECT_BINARY_NAME} PROPERTY APPIMAGE_DESKTOP_FILE ${CMAKE_BINARY_DIR}/stablediffusiongui.desktop)
set_property(TARGET ${PROJECT_BINARY_NAME} PROPERTY APPIMAGE_ICON_FILE ${CMAKE_SOURCE_DIR}/graphics/icons/256/stablediffusiongui.png)



install(TARGETS ${PROJECT_BINARY_NAME} DESTINATION ${bin_INSTALL_PATH_NSIS} COMPONENT "${CMAKE_PROJECT_NAME}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
#list(APPEND CPACK_COMPONENTS_ALL "${CMAKE_PROJECT_NAME}")

if (SD_SERVER OR EXISTS "${CMAKE_BINARY_DIR}/server/${EPREFIX}${PROJECT_BINARY_NAME}_server${CMAKE_EXECUTABLE_SUFFIX}")
    install(FILES ${CMAKE_BINARY_DIR}/server/${EPREFIX}${PROJECT_BINARY_NAME}_server${CMAKE_EXECUTABLE_SUFFIX}
    DESTINATION ${bin_INSTALL_PATH_NSIS}
    COMPONENT "${CMAKE_PROJECT_NAME}Server")
    list(APPEND CPACK_COMPONENTS_ALL "${CMAKE_PROJECT_NAME}Server")
    set(CPACK_DEBIAN_STABLEDIFFUSIONGUISERVER_PACKAGE_NAME "stablediffusiongui-server-${SDCPP_VERSION}")
    set(CPACK_DEBIAN_STABLEDIFFUSIONGUISERVER_PACKAGE_RELEASE "${DISTRO_VERSION}")
endif()


if (SDGUI_AVX OR EXISTS "${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_avx${CMAKE_SHARED_LIBRARY_SUFFIX}")
	install(FILES ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_avx${CMAKE_SHARED_LIBRARY_SUFFIX}
	DESTINATION ${lib_INSTALL_PATH_NSIS}
	COMPONENT libsdcpp_avx)
	list(APPEND CPACK_COMPONENTS_ALL "libsdcpp_avx")
    set(CPACK_DEBIAN_LIBSDCPP_AVX_PACKAGE_NAME "libstablediffusion-avx-${SDCPP_VERSION}")
    set(CPACK_DEBIAN_LIBSDCPP_AVX_PACKAGE_RELEASE "${DISTRO_VERSION}")
endif()

if (SDGUI_AVX2 OR EXISTS "${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_avx2${CMAKE_SHARED_LIBRARY_SUFFIX}")
	install(FILES ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_avx2${CMAKE_SHARED_LIBRARY_SUFFIX}
	DESTINATION ${lib_INSTALL_PATH_NSIS}
	COMPONENT libsdcpp_avx2)
	list(APPEND CPACK_COMPONENTS_ALL "libsdcpp_avx2")
    set(CPACK_DEBIAN_LIBSDCPP_AVX2_PACKAGE_NAME "libstablediffusion-avx2-${SDCPP_VERSION}")
    set(CPACK_DEBIAN_LIBSDCPP_AVX2_PACKAGE_RELEASE "${DISTRO_VERSION}")
endif()

if (SDGUI_AVX512 OR EXISTS "${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_avx512${CMAKE_SHARED_LIBRARY_SUFFIX}")
	install(FILES ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_avx512${CMAKE_SHARED_LIBRARY_SUFFIX}
	DESTINATION ${lib_INSTALL_PATH_NSIS}
	COMPONENT libsdcpp_avx512)
	list(APPEND CPACK_COMPONENTS_ALL "libsdcpp_avx512")
    set(CPACK_DEBIAN_LIBSDCPP_AVX512_PACKAGE_NAME "libstablediffusion-avx512-${SDCPP_VERSION}")
    set(CPACK_DEBIAN_LIBSDCPP_AVX512_PACKAGE_RELEASE "${DISTRO_VERSION}")
endif()

if (SDGUI_CUBLAS OR EXISTS "${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_cuda${CMAKE_SHARED_LIBRARY_SUFFIX}")
	install(FILES ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_cuda${CMAKE_SHARED_LIBRARY_SUFFIX}
	DESTINATION ${lib_INSTALL_PATH_NSIS}
	COMPONENT libsdcpp_cuda)
	list(APPEND CPACK_COMPONENTS_ALL "libsdcpp_cuda")
    set(CPACK_DEBIAN_LIBSDCPP_CUDA_PACKAGE_NAME "libstablediffusion-cuda-${SDCPP_VERSION}")
    set(CPACK_DEBIAN_LIBSDCPP_CUDA_PACKAGE_RELEASE "${DISTRO_VERSION}")
endif()

if (SDGUI_HIPBLAS OR EXISTS "${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_hipblas${CMAKE_SHARED_LIBRARY_SUFFIX}")
	install(FILES ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_hipblas${CMAKE_SHARED_LIBRARY_SUFFIX}
	DESTINATION ${lib_INSTALL_PATH_NSIS}
	COMPONENT libsdcpp_hipblas)
	list(APPEND CPACK_COMPONENTS_ALL "libsdcpp_hipblas")
    set(CPACK_DEBIAN_LIBSDCPP_HIPBLAS_PACKAGE_NAME "libstablediffusion-hipblas-${SDCPP_VERSION}")
    set(CPACK_DEBIAN_LIBSDCPP_HIPBLAS_PACKAGE_RELEASE "${DISTRO_VERSION}")
endif()

if (SDGUI_VULKAN OR EXISTS "${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_vulkan${CMAKE_SHARED_LIBRARY_SUFFIX}")
	install(FILES ${CMAKE_BINARY_DIR}/${EPREFIX}${CMAKE_SHARED_LIBRARY_PREFIX}stable-diffusion_vulkan${CMAKE_SHARED_LIBRARY_SUFFIX}
	DESTINATION ${lib_INSTALL_PATH_NSIS}
	COMPONENT libsdcpp_vulkan)
	list(APPEND CPACK_COMPONENTS_ALL "libsdcpp_vulkan")
    set(CPACK_DEBIAN_LIBSDCPP_VULKAN_PACKAGE_NAME "libstablediffusion-vulkan-${SDCPP_VERSION}")
    set(CPACK_DEBIAN_LIBSDCPP_VULKAN_PACKAGE_RELEASE "${DISTRO_VERSION}")
endif()


if(WIN32)
    set(CPACK_MODULE_PATH ${CMAKE_MODULE_PATH})
    list(APPEND CPACK_MODULE_PATH "${CMAKE_SOURCE_DIR}/platform/msvc")
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "${CMAKE_PROJECT_NAME}")
    set(CPACK_NSIS_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
    set(CPACK_NSIS_URL_INFO_ABOUT ${CMAKE_PROJECT_HOMEPAGE_URL})
    set(CPACK_NSIS_HELP_LINK "https://github.com/fszontagh/sd.cpp.gui.wx")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/graphics/icons/256/stablediffusiongui.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/graphics/icons/256/stablediffusiongui.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "StableDiffusionGUI.exe")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    SET(CPACK_NSIS_MUI_FINISHPAGE_RUN "StableDiffusionGUI.exe")
    set(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_NSIS_BRANDING_TEXT, "Stable Diffusion GUI")

    install(FILES ${CMAKE_SOURCE_DIR}/graphics/icons/256/stablediffusiongui.png DESTINATION ${bin_INSTALL_PATH_NSIS} COMPONENT "${CMAKE_PROJECT_NAME}")
    install(FILES ${CMAKE_SOURCE_DIR}/graphics/icons/256/stablediffusiongui.ico DESTINATION ${bin_INSTALL_PATH_NSIS} COMPONENT "${CMAKE_PROJECT_NAME}")


if (SD_SERVER)
    if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(FATAL_ERROR "Server is not supported yet in ${CMAKE_BUILD_TYPE} mode")
    endif()
    cmake_path(SET SERVICE_INSTALL_SCRIPT NORMALIZE ${CMAKE_BINARY_DIR}/nsis/nsis_install_service.nsh)
    cmake_path(SET SERVICE_UNINSTALL_SCRIPT NORMALIZE ${CMAKE_BINARY_DIR}/nsis/nsis_uninstall_service.nsh)



    configure_file(platform/msvc/nsis_install_service.nsh ${SERVICE_INSTALL_SCRIPT} @ONLY)
    configure_file(platform/msvc/nsis_uninstall_service.nsh ${SERVICE_UNINSTALL_SCRIPT} @ONLY)
    configure_file(platform/msvc/winsw.xml ${CMAKE_BINARY_DIR}/nsis/winsw.xml @ONLY)

    # download WinSW-x64 to manage as service
    file(DOWNLOAD https://github.com/winsw/winsw/releases/download/v3.0.0-alpha.11/WinSW-x64.exe ${CMAKE_BINARY_DIR}/nsis/WinSW-x64.exe)
    install(FILES ${CMAKE_BINARY_DIR}/nsis/WinSW-x64.exe DESTINATION ${bin_INSTALL_PATH_NSIS} COMPONENT "${CMAKE_PROJECT_NAME}Server")
    install(FILES ${CMAKE_BINARY_DIR}/nsis/winsw.xml DESTINATION ${bin_INSTALL_PATH_NSIS} COMPONENT "${CMAKE_PROJECT_NAME}Server")


    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "  !addincludedir ${CMAKE_BINARY_DIR}/nsis \n !include nsis_install_service.nsh")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "  !addincludedir ${CMAKE_BINARY_DIR}/nsis \n !include nsis_uninstall_service.nsh")
endif()


elseif(UNIX AND NOT APPLE)

    set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}-${DISTRO_VERSION}")

    string(TIMESTAMP TODAY "%Y%m%d")

    set(CPACK_GENERATOR "DEB")

    set(CPACK_DEB_COMPONENT_INSTALL ON)

    # DEB package settings
    set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ferenc Szontágh <szf@fsociety.hu>")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.29), libstdc++6 (>= 9)")
    set(CPACK_DEBIAN_STABLEDIFFUSIONGUI_PACKAGE_DEPENDS "libc6 (>= 2.29), libstdc++6 (>= 9), libgtk-3-0 (>= 3.9.10) | libgtk-4-1, libexiv2-27, libnotify4, openssl, curl, libudev1 (>= 183), libvulkan1, libx11-6, libstablediffusion-avx-${SDCPP_VERSION} (=${CPACK_PACKAGE_VERSION}-${DISTRO_VERSION}) | libstablediffusion-avx2-${SDCPP_VERSION} (=${CPACK_PACKAGE_VERSION}-${DISTRO_VERSION}) | libstablediffusion-avx512-${SDCPP_VERSION} (=${CPACK_PACKAGE_VERSION}-${DISTRO_VERSION}) | libstablediffusion-cuda-${SDCPP_VERSION} (=${CPACK_PACKAGE_VERSION}-${DISTRO_VERSION}) | libstablediffusion-hipblas-${SDCPP_VERSION} (=${CPACK_PACKAGE_VERSION}-${DISTRO_VERSION}) | libstablediffusion-vulkan-${SDCPP_VERSION} (=${CPACK_PACKAGE_VERSION}-${DISTRO_VERSION})")
    if (DISTRO_VER GREATER 23)
        set(CPACK_DEBIAN_LIBSDCPP_CUDA_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libcublas12, libcudart12, libcublaslt12, libnvidia-compute-470 | libnvidia-compute-535 | libnvidia-compute-550 ")
    else()
        set(CPACK_DEBIAN_LIBSDCPP_CUDA_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libcublas11, libcudart11, libcublaslt11, libnvidia-compute-470 | libnvidia-compute-535 | libnvidia-compute-550 ")
    endif()

    set(CPACK_DEBIAN_STABLEDIFFUSIONGUI_PACKAGE_CONFLICTS "stablediffusiongui-server")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CMAKE_PROJECT_DESCRIPTION})
    set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
    set(CPACK_DEBIAN_PACKAGE_RELEASE "${DISTRO_VERSION}")
    set(CPACK_DEBIAN_STABLEDIFFUSIONGUI_PACKAGE_NAME "stablediffusiongui")

    if (SD_SERVER)
        set(CPACK_DEBIAN_STABLEDIFFUSIONGUISERVER_PACKAGE_DEPENDS  "openssl, curl, libudev1 (>= 183), libvulkan1, libx11-6, libstablediffusion-avx-${SDCPP_VERSION} (=${CPACK_PACKAGE_VERSION}-${DISTRO_VERSION}) | libstablediffusion-avx2-${SDCPP_VERSION} (=${CPACK_PACKAGE_VERSION}-${DISTRO_VERSION}) | libstablediffusion-avx512-${SDCPP_VERSION} (=${CPACK_PACKAGE_VERSION}-${DISTRO_VERSION}) | libstablediffusion-cuda-${SDCPP_VERSION} (=${CPACK_PACKAGE_VERSION}-${DISTRO_VERSION}) | libstablediffusion-hipblas-${SDCPP_VERSION} (=${CPACK_PACKAGE_VERSION}-${DISTRO_VERSION}) | libstablediffusion-vulkan-${SDCPP_VERSION} (=${CPACK_PACKAGE_VERSION}-${DISTRO_VERSION})")
        set(CPACK_DEBIAN_STABLEDIFFUSIONGUISERVER_PACKAGE_CONFLICTS "stablediffusiongui")

        configure_file("platform/linux/postinst" ${CMAKE_BINARY_DIR}/postinst @ONLY)
        configure_file("platform/linux/prerm" ${CMAKE_BINARY_DIR}/prerm @ONLY)
        configure_file("platform/linux/postrm" ${CMAKE_BINARY_DIR}/postrm @ONLY)


        set(CPACK_DEBIAN_STABLEDIFFUSIONGUISERVER_PACKAGE_CONTROL_EXTRA
        "${CMAKE_BINARY_DIR}/postinst;${CMAKE_BINARY_DIR}/prerm;${CMAKE_BINARY_DIR}/postrm")

        configure_file("platform/linux/${PROJECT_BINARY_NAME}_server.service" ${CMAKE_BINARY_DIR}/server/${PROJECT_BINARY_NAME}_server.service @ONLY)
        install(FILES ${CMAKE_BINARY_DIR}/server/${PROJECT_BINARY_NAME}_server.service DESTINATION /lib/systemd/system COMPONENT "${CMAKE_PROJECT_NAME}Server")

    endif()


    set(APPIMAGE_SOURCE_DIR "${CMAKE_BINARY_DIR}/AppImageSource")


    # Generate the .desktop file by replacing placeholders
    configure_file(${CMAKE_SOURCE_DIR}/platform/linux/app.desktop ${CMAKE_BINARY_DIR}/stablediffusiongui.desktop @ONLY)

    install(FILES ${CMAKE_BINARY_DIR}/stablediffusiongui.desktop DESTINATION share/applications COMPONENT "${CMAKE_PROJECT_NAME}")
    install(FILES ${CMAKE_SOURCE_DIR}/graphics/icons/256/stablediffusiongui.png DESTINATION share/icons/hicolor/256x256/apps COMPONENT "${CMAKE_PROJECT_NAME}")
    include(${CMAKE_SOURCE_DIR}/cmake/AppImage.cmake)
    set(CPACK_COMPONENTS_GROUPING IGNORE)
elseif(APPLE)
    #set(CPACK_GENERATOR "DragNDrop")
    #set(CPACK_DMG_VOLUME_NAME "StableDiffusionGUI ${PROJECT_VERSION}")
    #set(CPACK_DMG_DS_STORE_FILE ${CMAKE_SOURCE_DIR}/platform/mac/ds_store)
    message(ERROR "macOS is not supported yet")
endif()

include(CPack)

    cpack_add_component("${CMAKE_PROJECT_NAME}"
        DISPLAY_NAME "SD C++ GUI"
        DESCRIPTION "Stable Diffusion CPP Desktop Graphical User Interface"
        CONFLICTS "${CMAKE_PROJECT_NAME}Server"
        INSTALL_TYPES "local"
    )

if (SD_SERVER)
    cpack_add_component("${CMAKE_PROJECT_NAME}Server"
        DISPLAY_NAME "SD C++ Server"
        DESCRIPTION "Stable Diffusion CPP Server"
        CONFLICTS "${CMAKE_PROJECT_NAME}"
        INSTALL_TYPES "server"
    )

    cpack_add_component("${CMAKE_PROJECT_NAME}Service"
        DISPLAY_NAME "Install server as a Windows service"
        DESCRIPTION "Run the Stable Diffusion server as a Windows service."
        DEPENDS "${CMAKE_PROJECT_NAME}Server"
        GROUP "Optional Features"
        INSTALL_TYPES "server"
    )
endif(SD_SERVER)

    cpack_add_component(libsdcpp_avx
        DISPLAY_NAME "SD C++ -AVX"
        DESCRIPTION "SD CPU backend with AVX CPU feature"
        GROUP backends
        INSTALL_TYPES "server" "local"
    )


    cpack_add_component(libsdcpp_avx2
        DISPLAY_NAME "SD C++ -AVX2"
        DESCRIPTION "SD CPU backend with AVX2 CPU feature"
        GROUP backends
        INSTALL_TYPES "server" "local"
    )

    cpack_add_component(libsdcpp_avx512
        DISPLAY_NAME "SD C++ -AVX512"
        DESCRIPTION "SD CPU backend with AVX512 CPU feature"
        GROUP backends
        INSTALL_TYPES "server" "local"
    )


    cpack_add_component(libsdcpp_cuda
        DISPLAY_NAME "SD C++ -CUDA"
        DESCRIPTION "SD GPU backend with CUDA GPU feature"
        GROUP backends
        INSTALL_TYPES "server" "local"
    )


    cpack_add_component(libsdcpp_hipblas
        DISPLAY_NAME "SD C++ -HIPBLAS"
        DESCRIPTION "SD GPU backend with AMD GPU feature"
        GROUP backends
        INSTALL_TYPES "server" "local"
    )


    cpack_add_component(libsdcpp_vulkan
        DISPLAY_NAME "SD C++ -VULKAN"
        DESCRIPTION "SD GPU backend with VULKAN feature"
        GROUP backends
        INSTALL_TYPES "server" "local"
    )

    cpack_add_component_group(backends
        DISPLAY_NAME "C++ Backends"
        EXPANDED
        DESCRIPTION "Stable Diffusion C++ Backends"
        INSTALL_TYPES "server" "local"
    )

    if (SD_SERVER)
    cpack_add_component_group("Optional Features"
            DISPLAY_NAME "Optional Features"
            EXPANDED
            DESCRIPTION "Stable Diffusion C++ Optional Features"
        )
    endif()


    cpack_add_install_type(local
        DISPLAY_NAME Local diffusion installation
    )
    cpack_add_install_type(server
        DISPLAY_NAME Server installation
    )




set(EPREFIX "")

if (WIN32)
	set(EPREFIX "${CMAKE_BUILD_TYPE}/")
endif()


message(STATUS "CPACK_COMPONENTS_ALL: ${CPACK_COMPONENTS_ALL}")
