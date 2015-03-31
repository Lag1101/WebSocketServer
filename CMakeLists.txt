cmake_minimum_required(VERSION 2.8.4)
project(WebSocketServer)

set(target WebSocketServer)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost REQUIRED)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -lboost_system")
set(SOURCE_FILES WebSocketServer.cpp Frame.cpp)


INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${Boost_INCLUDE_DIRS})
add_executable(${target} ${SOURCE_FILES} Session.cpp Session.h utils/Utils.cpp utils/Utils.h)
target_link_libraries(${target} ${Boost_LIBRARIES})