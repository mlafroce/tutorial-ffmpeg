#ifndef OUTPUTFORMAT_H
#define OUTPUTFORMAT_H
#include <string>

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
    // Escribe datos al azar;
    void writeData();
private:
    FormatContext& context;
    AVOutputFormat *avOutputFormat;
    AVStream *video_avstream;
    AVCodecContext* codecContext;
    FILE* outputFile;
};
#endif
