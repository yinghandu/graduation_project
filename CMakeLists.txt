cmake_minimum_required(VERSION 3.4.0)
project(dis)

set(CMAKE_CXX_STANDARD 17)
set(RESOURCE main.cpp cpp/server.cpp header/server.h cpp/worker.cpp header/worker.h cpp/Com.cpp header/Com.h cpp/scheduler.cpp header/scheduler.h cpp/Data.cpp header/Data.h header/Dis.h cpp/nodes.cpp header/nodes.h cpp/PostOffice.cpp header/PostOffice.h)

add_executable(dis ${RESOURCE})
