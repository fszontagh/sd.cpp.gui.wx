set(EPROCESS_BINARY_NAME "${PROJECT_BINARY_NAME}_diffuser")

math(EXPR SHARED_MEMORY_SIZE "1024*1024*16")
math(EXPR STACK_SIZE "${SHARED_MEMORY_SIZE} + ((1024*1024)*4)")
message(STATUS "SHARED_MEMORY_SIZE: ${SHARED_MEMORY_SIZE}")
message(STATUS "STACK_SIZE: ${STACK_SIZE}")

configure_file(src/config.hpp.in config.hpp)

set(SOURCES src/main.cpp ${CMAKE_SOURCE_DIR}/src/helpers/QueueItem.cpp)

if(MSVC)
    set(BNAME ${EPROCESS_BINARY_NAME})
    set(COMPONENT_NAME "${PROJECT_DISPLAY_NAME} Diffuser")
    configure_file(../platform/msvc/app.rc.in app.rc)
    configure_file(../platform/msvc/minimal.rc.in minimal.rc)
    list(APPEND SOURCES minimal.rc)
    list(APPEND SOURCES ${CMAKE_CURRENT_BINARY_DIR}/app.rc)
    list(APPEND SOURCES ${CMAKE_CURRENT_BINARY_DIR}/minimal.rc)
endif()

add_executable(${EPROCESS_BINARY_NAME}
    ${SOURCES}
    ${CMAKE_SOURCE_DIR}/src/libs/SharedLibrary.cpp
    ${CMAKE_SOURCE_DIR}/src/libs/SharedMemoryManager.cpp
    src/ApplicationLogic.cpp
    src/stb.cpp
)

target_precompile_headers(${EPROCESS_BINARY_NAME} PRIVATE src/pch.h)

if(MSVC)
    target_link_options(${EPROCESS_BINARY_NAME} PRIVATE /STACK:${STACK_SIZE})
    target_compile_options(${EPROCESS_BINARY_NAME} PRIVATE /Zc:__cplusplus)
else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,stack-size=${STACK_SIZE}")
endif()

target_include_directories(${EPROCESS_BINARY_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${wxWidgets_SOURCE_DIR}/include
    ${exiv2_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(bin_INSTALL_PATH_NSIS "bin")

if(WIN32)
    set(bin_INSTALL_PATH_NSIS ".")
endif()

install(TARGETS ${EPROCESS_BINARY_NAME}
    RUNTIME DESTINATION ${bin_INSTALL_PATH_NSIS}
    COMPONENT "${CMAKE_PROJECT_NAME}"
    PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE
)

if(WIN32)
    target_link_libraries(${EPROCESS_BINARY_NAME} PRIVATE wxWidgets::wxWidgets OpenSSL::Crypto)
else()
    target_link_libraries(${EPROCESS_BINARY_NAME} PRIVATE wx::base wx::core OpenSSL::Crypto)
endif()

if(SDGUI_AVX)
    list(APPEND DEPENDENCIES stable_diffusion_cpp_avx)
endif()

if(SDGUI_AVX2)
    list(APPEND DEPENDENCIES stable_diffusion_cpp_avx2)
endif()

if(SDGUI_AVX512)
    list(APPEND DEPENDENCIES stable_diffusion_cpp_avx512)
endif()

if(SDGUI_CUDA)
    list(APPEND DEPENDENCIES stable_diffusion_cpp_cuda)
endif()

if(SDGUI_VULKAN)
    list(APPEND DEPENDENCIES stable_diffusion_cpp_vulkan)
endif()

if(SDGUI_HIPBLAS)
    list(APPEND DEPENDENCIES stable_diffusion_cpp_hipblas)
endif()

add_dependencies(${EPROCESS_BINARY_NAME} ${DEPENDENCIES})
