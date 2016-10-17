cmake_minimum_required(VERSION 3.3)
project(Refuse)

find_package(Threads)

include_directories(include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
add_executable(Refuse src/main.c include/refuse/heap.h src/heap.c)
target_link_libraries(Refuse ${CMAKE_THREAD_LIBS_INIT})
