configure_file(ver.hpp.in ver.hpp)

set(SOURCES
    main.cpp
    ui/MainWindow.cpp
    ui/MainWindowAboutDialog.cpp
    ui/MainWindowSettings.cpp
    ui/MainWindowCivitAiWindow.cpp
    ui/MainWindowImageDialog.cpp
    ui/MainWindowUI.cpp
    ui/MainWindowDesktopWidget.cpp
    ui/QueueManager.cpp
    ui/ModelInfo.cpp
    libs/SharedMemoryManager.cpp
    libs/TcpClient.cpp
    helpers/QueueItem.cpp
    helpers/DataViewListManager.cpp
    helpers/ModelUiManager.cpp
)

add_executable(${PROJECT_BINARY_NAME} ${SOURCES})
target_precompile_headers(${PROJECT_BINARY_NAME} PRIVATE pch.h)

target_include_directories(${PROJECT_BINARY_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}
    ${CpuFeatures_SOURCE_DIR}
    ${sockets_cpp_SOURCE}
)

set_target_properties(${PROJECT_BINARY_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

if (wxWidgets_INCLUDE_DIRS)
    target_include_directories(${PROJECT_BINARY_NAME} PRIVATE ${wxWidgets_INCLUDE_DIRS})
endif()

if (NOT WIN32)
    set(APPDEPENDS wx::base wx::core wx::xrc wx::richtext wx::stc)
else()
    set(APPDEPENDS wxWidgets::wxWidgets)
    if (MSVC)
        set_target_properties(${PROJECT_BINARY_NAME} PROPERTIES
            COMPILE_FLAGS "/DwxUSE_RC_MANIFEST"
            LINK_FLAGS "/MANIFEST:NO"
        )
    endif()
endif()

list(APPEND APPDEPENDS CpuFeatures::cpu_features OpenSSL::Crypto CURL::libcurl exiv2lib)


target_link_libraries(${PROJECT_BINARY_NAME} PRIVATE ${APPDEPENDS})

add_dependencies(${PROJECT_BINARY_NAME} sockets_cpp po-compile)
if (WIN32)
    target_link_libraries(${PROJECT_BINARY_NAME} PRIVATE comsuppw.lib wbemuuid.lib)
    target_compile_definitions(${PROJECT_BINARY_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _UNICODE
        UNICODE
        WIN32
        _WINDOWS
        wxUSE_RC_MANIFEST=1
        __WXMSW__
    )
    set_property(TARGET ${PROJECT_BINARY_NAME} PROPERTY WIN32_EXECUTABLE TRUE)
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(${PROJECT_BINARY_NAME} PRIVATE
        __WXGTK__
        _FILE_OFFSET_BITS=64
    )
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    target_include_directories(${PROJECT_BINARY_NAME} PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_BINARY_NAME} PRIVATE ${GTK3_LIBRARIES})
endif()