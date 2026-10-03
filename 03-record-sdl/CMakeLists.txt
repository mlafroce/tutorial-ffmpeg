cmake_minimum_required(VERSION 3.5)

project(ffmpeg-sdl)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-ggdb)

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
  src/OutputFormat.cpp
  src/FormatContext.cpp
  src/main.cpp
)

target_link_libraries(record-sdl avformat avcodec avutil swscale sdldemo SDL2 SDL2_image)

# Copio la imagen junto al ejecutable, que la busca en su propia carpeta
configure_file(assets/cat.gif ${CMAKE_CURRENT_BINARY_DIR}/cat.gif COPYONLY)
