#include "FormatContext.h"
#include <stdexcept>
#include <string>
extern "C" {
#include <libavformat/avformat.h>
}

FormatContext::FormatContext() {
    this->pFormatCtx = avformat_alloc_context();
}

FormatContext::~FormatContext() {
    avformat_free_context(this->pFormatCtx);
}


void FormatContext::dumpFormat(const std::string& filename) {
    // Abro el video, el contexto será del tipo `input`
    if(avformat_open_input(&this->pFormatCtx, filename.c_str(), NULL, NULL) != 0)
        throw std::runtime_error("Error al abrir archivo");

    /// Obtiene informacion del stream
    if(avformat_find_stream_info(this->pFormatCtx, NULL) < 0) {
        throw std::runtime_error("Error obtener informacion");
    }
    
    /// Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, filename.c_str(), 0);
    avformat_close_input(&this->pFormatCtx);
}
