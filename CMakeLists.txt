cmake_minimum_required(VERSION 3.3)
project(mycompiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        Graph.cpp
        Graph.h
        GraphNode.cpp
        GraphNode.h
        LineLink.cpp
        LineLink.h
        main.cpp
        NfaToDfa.cpp
        NfaToDfa.h
        RexHandle.cpp
        RexHandle.h Token.cpp Token.h Parse.cpp Parse.h)

add_executable(mycompiler ${SOURCE_FILES})
