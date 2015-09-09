cmake_minimum_required(VERSION 2.8.4)
project(WebSocketServer)

set(target WebSocketServer)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost REQUIRED)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -lboost_system")
set(SOURCE_FILES WebSocketServer.cpp Frame.cpp boost_http_server/mime_types.cpp boost_http_server/mime_types.hpp boost_http_server/header.hpp boost_http_server/connection.cpp boost_http_server/connection.hpp boost_http_server/request_handler.cpp boost_http_server/request_handler.hpp boost_http_server/connection_manager.cpp boost_http_server/connection_manager.hpp boost_http_server/server.cpp boost_http_server/server.hpp boost_http_server/reply.cpp boost_http_server/reply.hpp boost_http_server/boost_http_server_entry.hpp boost_http_server/request_parser.cpp boost_http_server/request_parser.hpp boost_http_server/request.hpp)


INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${Boost_INCLUDE_DIRS})
add_executable(${target} ${SOURCE_FILES} Session.cpp Session.h utils/Utils.cpp utils/Utils.h)
target_link_libraries(${target} ${Boost_LIBRARIES})