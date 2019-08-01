cmake_minimum_required(VERSION 3.0)

SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
PROJECT(Threadpool)
add_executable( Threadpool
                main.cpp
				Threadpool.cpp
				Threadpool.h
				)
