cmake_minimum_required(VERSION 2.6)
project(ffmpeg-demo) 

set(CMAKE_CXX_FLAGS
 "${CMAKE_CXX_FLAGS} -ggdb -std=c++11 -Wall -Werror -pedantic -pedantic-errors")

add_executable(write-video
	main.cpp
	FormatContext.cpp
	OutputFormat.cpp
)

target_link_libraries(write-video avformat avcodec avutil)
