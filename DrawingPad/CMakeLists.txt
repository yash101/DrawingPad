project(DrawingPad)

file(COPY "res" DESTINATION ".")
include(dlib/cmake)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
aux_source_directory(source SRC_LIST)
file(GLOB HEAD_LIST "include/*.hxx")
find_package(Threads REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -g")
add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEAD_LIST})
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} dlib)
