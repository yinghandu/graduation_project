cmake_minimum_required(VERSION 3.4.0)
project(dis)

add_compile_options(-g)
set(CMAKE_CXX_STANDARD 14)
set(RESOURCE main.cpp cpp/server.cpp header/server.h cpp/worker.cpp header/worker.h cpp/Com.cpp header/Com.h cpp/scheduler.cpp header/scheduler.h cpp/Data.cpp header/Data.h header/Dis.h cpp/nodes.cpp header/nodes.h cpp/PostOffice.cpp header/PostOffice.h cpp/LogisticRegression.cpp header/LogisticRegression.h)


find_package(Threads)

add_definitions("-g -std=c++11 -lpthread")

add_executable(dis ${RESOURCE})

target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
