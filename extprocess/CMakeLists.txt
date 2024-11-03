set(EPROCESS_BINARY_NAME "${PROJECT_BINARY_NAME}_diffuser")

configure_file(src/config.hpp.in config.hpp)


add_executable(${EPROCESS_BINARY_NAME} src/main.cpp ${CMAKE_SOURCE_DIR}/src/libs/SharedLibrary.cpp ${CMAKE_SOURCE_DIR}/src/libs/SharedMemoryManager.cpp src/ApplicationLogic.cpp)

target_include_directories(${EPROCESS_BINARY_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src ${wxWidgets_SOURCE_DIR}/include ${exiv2_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
install(TARGETS ${EPROCESS_BINARY_NAME} RUNTIME DESTINATION bin COMPONENT "${CMAKE_PROJECT_NAME}")

if(WIN32)
    target_link_libraries(${EPROCESS_BINARY_NAME} PRIVATE  wxWidgets::wxWidgets exiv2lib)
else(WIN32)
    target_link_libraries(${EPROCESS_BINARY_NAME} PRIVATE wx::base wx::core exiv2lib)
endif(WIN32)