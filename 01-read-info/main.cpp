/// Código extraido de https://github.com/phamquy/FFmpeg-tutorial-samples/
#include <iostream>
#include <stdexcept>
#include "FormatContext.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "Please provide a movie file" << std::endl;
        return 1;
    }

    /// Inicializo contexto
    try {
        FormatContext context;
        context.dumpFormat(argv[1]);
    } catch (const std::runtime_error& re) {
        std::cerr << re.what() << std::endl;
        return 1;
    }
    return 0;
}
