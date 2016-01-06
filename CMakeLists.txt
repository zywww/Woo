cmake_minimum_required(VERSION 3.3)
project(Woo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        Regex/Graph.cpp
        Regex/Graph.h
        Regex/GraphNode.cpp
        Regex/GraphNode.h
        Regex/LineLink.cpp
        Regex/LineLink.h
        main.cpp
        Regex/NfaToDfa.cpp
        Regex/NfaToDfa.h
        Regex/RexHandle.cpp
        Regex/RexHandle.h Token.cpp Token.h Parse.cpp Parse.h)

add_executable(Woo ${SOURCE_FILES})
