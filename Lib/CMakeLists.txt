cmake_minimum_required(VERSION 3.22)
project(Lib)

set(CMAKE_CXX_STANDARD 14)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_definitions(-DUNICODE -D_UNICODE)
endif ()

add_executable(Lib main.cpp
        schcon/base.h
        schcon/date.h
        schcon/time.h
        schcon/calendar.h
        schcon/task.h
        schcon/escapist/flag.h
)
