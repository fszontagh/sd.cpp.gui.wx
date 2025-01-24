set(SERVER_BINARY_NAME "${PROJECT_BINARY_NAME}_server")

set(SOURCES src/main.cpp src/TerminalApp.cpp src/SocketApp.cpp)

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


if (WIN32)
	set(bin_INSTALL_PATH_NSIS ".")
    set(share_INSTALL_PATH_NSIS ".")
endif()
# to generate Dockerfile for the server
configure_file(platforms/Docker/Dockerfile.in ${PROJECT_BINARY_DIR}/Dockerfile)
configure_file(server-config-example.json ${PROJECT_BINARY_DIR}/server-config-example.json @ONLY)

install(TARGETS ${SERVER_BINARY_NAME} RUNTIME DESTINATION ${bin_INSTALL_PATH_NSIS} COMPONENT "${CMAKE_PROJECT_NAME}Server")
install(FILES ${PROJECT_BINARY_DIR}/Dockerfile DESTINATION ${share_INSTALL_PATH_NSIS} COMPONENT "${CMAKE_PROJECT_NAME}Server")

if(WIN32)
    target_link_libraries(${SERVER_BINARY_NAME} PRIVATE  wxWidgets::wxWidgets OpenSSL::Crypto)
else(WIN32)
    target_link_libraries(${SERVER_BINARY_NAME} PRIVATE wx::base wx::core OpenSSL::Crypto)
endif(WIN32)