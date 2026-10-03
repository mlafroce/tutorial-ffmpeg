#ifndef OUTPUTFORMAT_H
#define OUTPUTFORMAT_H
#include <string>

struct AVCodec;
struct AVFrame;
struct AVPacket;
struct AVOutputFormat;
struct AVStream;
struct AVCodecContext;
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
    // No copiable, es dueño del codec y del archivo de salida
    OutputFormat(const OutputFormat&) = delete;
    OutputFormat& operator=(const OutputFormat&) = delete;
    // Escribe un video con datos de prueba
    void writeData();
private:
    // Genera un frame con datos de prueba
    void drawFrame(AVFrame* frame, int i);
    // Inicializa valores del codecContext
    void codecContextInit(const AVCodec* codec);
    // Crea el stream de video, abre el archivo y escribe el header
    void openOutput(const std::string& outputName);
    // Codifica un frame y escribe los paquetes resultantes en el contenedor
    void encode(AVFrame* frame, AVPacket* pkt);
    // Libera los recursos reservados
    void release();
    FormatContext& context;
    const AVOutputFormat *avOutputFormat;
    AVStream *videoStream;
    AVCodecContext* codecContext;
    bool headerWritten;
    bool trailerWritten;
};
#endif
