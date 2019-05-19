cmake_minimum_required(VERSION 2.6)
project(ffmpeg-demo) 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-std=c++11 -Wall -Werror -pedantic -pedantic-errors")

# Los siguiente es equivalente a
# g++ main.cpp -o readinfo
add_executable(read-info
	main.cpp
	FormatContext.cpp
)

# Lo siguiente es equivalente a agregar el flag -lavformat 
target_link_libraries(read-info avformat)
