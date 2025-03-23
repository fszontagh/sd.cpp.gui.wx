set(SERVER_BINARY_NAME "${PROJECT_BINARY_NAME}_server")

set(SOURCES
    src/main.cpp
    src/TerminalApp.cpp
    src/SocketApp.cpp
    src/stb.cpp
    ${CMAKE_SOURCE_DIR}/src/helpers/QueueItem.cpp
    )

if (MSVC)
    set(BNAME ${SERVER_BINARY_NAME})
    set(COMPONENT_NAME "${PROJECT_DISPLAY_NAME} Server")
    configure_file(../platform/msvc/app.rc.in app.rc)
    configure_file(../platform/msvc/minimal.rc.in minimal.rc)
    list(APPEND SOURCES minimal.rc)
    list(APPEND SOURCES ${CMAKE_CURRENT_BINARY_DIR}/app.rc)
    list(APPEND SOURCES ${CMAKE_CURRENT_BINARY_DIR}/minimal.rc)
endif()

add_executable(${SERVER_BINARY_NAME} ${SOURCES} ${CMAKE_SOURCE_DIR}/src/libs/SharedLibrary.cpp ${CMAKE_SOURCE_DIR}/src/libs/SharedMemoryManager.cpp)
add_dependencies(${SERVER_BINARY_NAME} sockets_cpp)
target_precompile_headers(${SERVER_BINARY_NAME} PRIVATE src/pch.h)

if (MSVC)
    target_link_options(${SERVER_BINARY_NAME} PRIVATE /SUBSYSTEM:WINDOWS /STACK:${STACK_SIZE})
    target_compile_options(${SERVER_BINARY_NAME} PRIVATE /Zc:__cplusplus)
    #target_compile_definitions(${SERVER_BINARY_NAME} PRIVATE WXUSINGDLL _UNICODE)

else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,stack-size=${STACK_SIZE}")
endif()



target_include_directories(${SERVER_BINARY_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/server/src ${CMAKE_SOURCE_DIR}/src ${sockets_cpp_SOURCE} ${wxWidgets_SOURCE_DIR}/include ${exiv2_INCLUDE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})


set(bin_INSTALL_PATH_NSIS "bin")
set(share_INSTALL_PATH_NSIS "share/stablediffusiongui")
set(etc_INSTALL_PATH_NSIS "/etc/stablediffusiongui_server")


if (WIN32)
	set(bin_INSTALL_PATH_NSIS ".")
    set(share_INSTALL_PATH_NSIS ".")
    set(share_INSTALL_PATH_NSIS ".")
else()
    set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)
endif()
# to generate Dockerfile for the server
configure_file(platforms/Docker/Dockerfile.in ${CMAKE_CURRENT_BINARY_DIR}/Dockerfile @ONLY)
configure_file(server-config-example.json ${CMAKE_CURRENT_BINARY_DIR}/server.json @ONLY)
configure_file(server-config-example-docker.json ${CMAKE_CURRENT_BINARY_DIR}/server-docker.json @ONLY)
configure_file(platforms/Docker/download_base_models.sh ${CMAKE_CURRENT_BINARY_DIR}/download_base_models.sh COPYONLY)
configure_file(platforms/Docker/entrypoint.sh ${CMAKE_CURRENT_BINARY_DIR}/entrypoint.sh @ONLY)

install(TARGETS ${SERVER_BINARY_NAME} RUNTIME DESTINATION ${bin_INSTALL_PATH_NSIS} COMPONENT "${CMAKE_PROJECT_NAME}Server" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
install(TARGETS ${PROJECT_BINARY_NAME}_diffuser RUNTIME DESTINATION ${bin_INSTALL_PATH_NSIS} COMPONENT "${CMAKE_PROJECT_NAME}Server" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/server.json DESTINATION ${share_INSTALL_PATH_NSIS} COMPONENT "${CMAKE_PROJECT_NAME}Server")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/download_base_models.sh DESTINATION ${share_INSTALL_PATH_NSIS} COMPONENT "${CMAKE_PROJECT_NAME}Server")


if(WIN32)
    target_link_libraries(${SERVER_BINARY_NAME} PRIVATE  wxWidgets::wxWidgets OpenSSL::Crypto)
else(WIN32)
    target_link_libraries(${SERVER_BINARY_NAME} PRIVATE wx::base OpenSSL::Crypto)
endif(WIN32)

add_custom_command(
    TARGET ${SERVER_BINARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${PROJECT_BINARY_NAME}_diffuser>
        $<TARGET_FILE_DIR:${SERVER_BINARY_NAME}>
)