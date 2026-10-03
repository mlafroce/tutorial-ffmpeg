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
struct SwsContext;
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
    // No copiable, es dueño del codec, el frame y el archivo de salida
    OutputFormat(const OutputFormat&) = delete;
    OutputFormat& operator=(const OutputFormat&) = delete;
    // Escribe un frame a disco. Utiliza `swsContext` para convertir
    // de RGB24 a YUV420p
    void writeFrame(const char* data, SwsContext* swsContext);
    // Vacía el encoder y cierra el contenedor de video
    void close();
private:
    // Inicializa frame
    void initFrame();
    // Inicializa contexto de codec
    void codecContextInit(const AVCodec* codec);
    // Crea el stream de video, abre el archivo y escribe el header
    void openOutput(const std::string& outputName);
    // Codifica un frame y escribe los paquetes resultantes en el contenedor
    void encode(AVFrame* avFrame);
    // Libera los recursos reservados
    void release();
    FormatContext& context;
    const AVOutputFormat* avOutputFormat;
    AVStream* videoStream;
    AVCodecContext* codecContext;
    int currentPts;
    AVFrame* frame;
    AVPacket* pkt;
    bool headerWritten;
    bool trailerWritten;
};
#endif
