/// Código extraido de https://github.com/phamquy/FFmpeg-tutorial-samples/
#include <iostream>
#include <stdexcept>
extern "C" {
#include <libavformat/avformat.h>
}
#include "FormatContext.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Please provide a movie file\n");
        return -1;
    }
    
    // Deprecado en ffmpeg >4
    // Esta función posee varios leaks, por lo que es útil hacerse un suppression file
    // con la misma
    av_register_all();
    
    /// Inicializo contexto
    try {
    FormatContext context;
    context.dumpFormat(argv[1]);
    } catch (const std::runtime_error& re) {
        std::cout << re.what() << std::endl;
    }
    return 0;
}
