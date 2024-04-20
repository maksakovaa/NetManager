cmake_minimum_required(VERSION 3.5.0)
project(tcp_server VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../)
#link_libraries(ws2_32 wsock32)
add_executable(tcp_server src/settings.cpp src/Logger.cpp src/net.cpp src/menu.cpp src/main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
