cmake_minimum_required(VERSION 3.5)
project(ffmpeg-demo) 

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(write-video
	main.cpp
	FormatContext.cpp
	OutputFormat.cpp
)

target_compile_options(write-video PRIVATE -ggdb -Wall -Werror -pedantic -pedantic-errors)

target_link_libraries(write-video avformat avcodec avutil)
