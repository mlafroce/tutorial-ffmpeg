cmake_minimum_required(VERSION 2.6)
project(ffmpeg-demo) 

set(CMAKE_CXX_FLAGS
 "${CMAKE_CXX_FLAGS} -ggdb -std=c++11 -Wall -pedantic -pedantic-errors")

# Los siguiente es equivalente a
# g++ main.cpp -o readinfo
add_executable(writevideo
	main.cpp
	FormatContext.cpp
	OutputFormat.cpp
)

# Lo siguiente es equivalente a agregar el flag -lavformat 
target_link_libraries(writevideo avformat avcodec avutil)
