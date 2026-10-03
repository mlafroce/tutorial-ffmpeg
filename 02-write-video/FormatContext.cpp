#include "FormatContext.h"
#include <stdexcept>
extern "C" {
#include <libavformat/avformat.h>
}

FormatContext::FormatContext() {
    this->pFormatCtx = avformat_alloc_context();
    if (!this->pFormatCtx) {
        throw std::runtime_error("No se pudo reservar memoria para el contexto");
    }
}

FormatContext::~FormatContext() {
    avformat_free_context(this->pFormatCtx);
}

AVFormatContext * FormatContext::getContext() const {
    return this->pFormatCtx;
}
