cmake_minimum_required(VERSION 2.6)

project(ffmpeg-sdl)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -std=c++11")

add_library(sdldemo
        src/Area.cpp
        src/SdlTexture.cpp
        src/SdlException.cpp
        src/SdlWindow.cpp
)

# Agrego la carpeta con los headers autogenerados a los includes
message("Agregando '${PROJECT_BINARY_DIR}' como directorio de includes")
include_directories(${PROJECT_BINARY_DIR})

add_executable (record-sdl
        src/Output.cpp
        src/Frame.cpp
        src/Packet.cpp
        src/FormatOutput.cpp
        src/CodecContext.cpp
        src/FormatContext.cpp
        src/SwsContext.cpp
        src/BlockingQueue.cpp
        src/Consumer.cpp
        src/Thread.cpp
        src/main.cpp
)

target_link_libraries(record-sdl avformat avcodec avutil swscale sdldemo SDL2 SDL2_image)

install(FILES assets/cat.gif DESTINATION ${CMAKE_BINARY_DIR})
