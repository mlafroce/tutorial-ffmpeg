#ifndef OUTPUTFORMAT_H
#define OUTPUTFORMAT_H
#include <string>

class AVCodec;
class AVFrame;
class AVOutputFormat;
class AVStream;
class AVCodecContext;
class FormatContext;
/**
 * Clase que encapsula lógica la salida de video
 * Se recomienda modularizar aun más esta clase, reforzando RAII
 */
class OutputFormat {
public:
    // Ctor
    OutputFormat(FormatContext& context, const std::string& filename);
    // Dtor
    ~OutputFormat();
    // Escribe un video con datos de prueba
    void writeData();
private:
    // Genera un frame con datos de prueba
    void drawFrame(AVFrame* frame, int i);
    // Inicializa valores del codecContext
    void codecContextInit(AVCodec* codec);
    FormatContext& context;
    AVOutputFormat *avOutputFormat;
    AVStream *video_avstream;
    AVCodecContext* codecContext;
    FILE* outputFile;
};
#endif
