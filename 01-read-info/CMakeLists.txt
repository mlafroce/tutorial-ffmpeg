cmake_minimum_required(VERSION 3.5)
project(ffmpeg-demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Los siguiente es equivalente a
# g++ main.cpp FormatContext.cpp -o read-info
add_executable(read-info
	main.cpp
	FormatContext.cpp
)

target_compile_options(read-info PRIVATE -Wall -Werror -pedantic -pedantic-errors)

# Lo siguiente es equivalente a agregar el flag -lavformat
target_link_libraries(read-info avformat)
