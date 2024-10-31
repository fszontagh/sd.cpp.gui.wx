set(EPROCESS_BINARY_NAME "${PROJECT_BINARY_NAME}_diffuser")

configure_file(src/config.hpp.in config.hpp)


add_executable(${EPROCESS_BINARY_NAME} src/main.cpp ../src/libs/SharedLibrary.cpp ../src/libs/SharedMemoryManager.cpp src/ApplicationLogic.cpp)

target_include_directories(${EPROCESS_BINARY_NAME} PRIVATE ../src ${wxWidgets_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
    target_link_libraries(${EPROCESS_BINARY_NAME} PRIVATE  wxWidgets::wxWidgets)
else(WIN32)
    target_link_libraries(${EPROCESS_BINARY_NAME} PRIVATE wx::base wx::core)
endif(WIN32)