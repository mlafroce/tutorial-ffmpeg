/// Código extraido de https://github.com/phamquy/FFmpeg-tutorial-samples/
#include <iostream>
#include <stdexcept>
extern "C" {
#include <libavformat/avformat.h>
}
#include "FormatContext.h"
#include "OutputFormat.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Please provide a movie file\n");
        return -1;
    }
    
    av_register_all();
    
    /// Inicializo contexto
    try {
        FormatContext context;
        OutputFormat output(context, argv[1]);
        output.writeData();
    } catch (const std::runtime_error& re) {
        std::cout << re.what() << std::endl;
    }
    return 0;
}
