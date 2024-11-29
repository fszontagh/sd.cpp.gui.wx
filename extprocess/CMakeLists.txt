set(EPROCESS_BINARY_NAME "${PROJECT_BINARY_NAME}_diffuser")

math(EXPR SHARED_MEMORY_SIZE "1024*1024*24")
message(STATUS "SHARED_MEMORY_SIZE: ${SHARED_MEMORY_SIZE}")


configure_file(src/config.hpp.in config.hpp)


set(SOURCES "src/main.cpp")

if (MSVC)
    list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/minimal.rc)
endif()

add_executable(${EPROCESS_BINARY_NAME} ${SOURCES} ${CMAKE_SOURCE_DIR}/src/libs/SharedLibrary.cpp ${CMAKE_SOURCE_DIR}/src/libs/SharedMemoryManager.cpp src/ApplicationLogic.cpp)


if (MSVC)
    math(EXPR STACK_SIZE "${SHARED_MEMORY_SIZE} + ((1024*1024)*4)")
    message(STATUS "STACK_SIZE: ${STACK_SIZE}")
    target_link_options(${EPROCESS_BINARY_NAME} PRIVATE /STACK:${STACK_SIZE})
    target_compile_options(${EPROCESS_BINARY_NAME} PRIVATE /Zc:__cplusplus)
endif()

target_include_directories(${EPROCESS_BINARY_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src ${wxWidgets_SOURCE_DIR}/include ${exiv2_INCLUDE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})


set(bin_INSTALL_PATH_NSIS "bin")


if (WIN32)
	set(bin_INSTALL_PATH_NSIS ".")
endif()


install(TARGETS ${EPROCESS_BINARY_NAME} RUNTIME DESTINATION ${bin_INSTALL_PATH_NSIS} COMPONENT "${CMAKE_PROJECT_NAME}")

if(WIN32)
    target_link_libraries(${EPROCESS_BINARY_NAME} PRIVATE  wxWidgets::wxWidgets exiv2lib)
else(WIN32)
    target_link_libraries(${EPROCESS_BINARY_NAME} PRIVATE wx::base wx::core exiv2lib)
endif(WIN32)