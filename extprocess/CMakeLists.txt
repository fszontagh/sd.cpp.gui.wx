set(EPROCESS_BINARY_NAME "extprocess")

add_executable(${EPROCESS_BINARY_NAME} src/main.cpp ../src/libs/SharedLibrary.cpp)

target_include_directories(${EPROCESS_BINARY_NAME} PRIVATE ../src)