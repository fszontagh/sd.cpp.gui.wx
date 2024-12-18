ExternalProject_Add(
  sockets_cpp
  PREFIX ${CMAKE_BINARY_DIR}/external/sockets_cpp
  GIT_REPOSITORY https://github.com/CJLove/sockets-cpp.git
  GIT_TAG master
  CMAKE_ARGS -DBUILD_TESTS=OFF -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_LIBS=ON -DFMT_SUPPORT=OFF
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
)
ExternalProject_Get_property(sockets_cpp SOURCE_DIR)
set(sockets_cpp_SOURCE ${SOURCE_DIR}/include)
message(STATUS "sockets_cpp_SOURCE: ${sockets_cpp_SOURCE}")