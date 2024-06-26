cmake_minimum_required(VERSION 3.5.0)
project(tcp_server VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../)

set(TCP_BINARY src/main.cpp)
set(TCP_NET_LIB src/net.h src/net.cpp)
set(TCP_API_LIB src/reqAPI.h src/reqAPI.cpp)
set(TCP_CONF_LIB src/settings.h src/settings.cpp)
set(TCP_MENU_LIB src/menu.h src/menu.cpp)
set(TCP_LOGGER_LIB src/Logger.h src/Logger.cpp)

add_library(netLib SHARED ${TCP_NET_LIB})
add_library(apiLib SHARED ${TCP_API_LIB})
add_library(confLib SHARED ${TCP_CONF_LIB})
add_library(menLib SHARED ${TCP_MENU_LIB})
add_library(logLib SHARED ${TCP_LOGGER_LIB})
if (WIN32)
    link_libraries(ws2_32 wsock32)
endif()
add_executable(tcp_server ${TCP_BINARY})
target_link_libraries(tcp_server menLib netLib apiLib confLib logLib)
