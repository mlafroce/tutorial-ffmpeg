#include "OutputFormat.h"
#include "FormatContext.h"
#include <memory>
#include <string>
#include <stdexcept>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

// Liberan frame y paquete aunque se lance una excepción
struct FrameDeleter {
    void operator()(AVFrame* frame) const { av_frame_free(&frame); }
};

struct PacketDeleter {
    void operator()(AVPacket* pkt) const { av_packet_free(&pkt); }
};

// Extensión que se agrega cuando el nombre de archivo no trae una.
static const char* DEFAULT_EXTENSION = ".mp4";

// Devuelve el nombre de archivo con la extensión por defecto si no tiene una
static std::string withExtension(const std::string& filename) {
    size_t nameStart = filename.find_last_of('/');
    nameStart = (nameStart == std::string::npos) ? 0 : nameStart + 1;
    size_t dot = filename.find_last_of('.');
    bool hasExtension = dot != std::string::npos && dot > nameStart
                        && dot + 1 < filename.size();
    return hasExtension ? filename : filename + DEFAULT_EXTENSION;
}

/**
 * Codifica un frame y escribe los paquetes resultantes en el contenedor
 */
void OutputFormat::encode(AVFrame *frame, AVPacket *pkt)
{
    /* send the frame to the encoder */
    int ret = avcodec_send_frame(this->codecContext, frame);
    if (ret < 0) {
        throw std::runtime_error("Error al enviar frame");
    }
    while (ret >= 0) {
        ret = avcodec_receive_packet(this->codecContext, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            throw std::runtime_error("Error al codificar");
        }
        // Cada paquete dura un frame
        if (pkt->duration == 0) {
            pkt->duration = 1;
        }
        // El contenedor puede usar una base de tiempo distinta a la del codec
        av_packet_rescale_ts(pkt, this->codecContext->time_base,
                             this->videoStream->time_base);
        pkt->stream_index = this->videoStream->index;
        // av_interleaved_write_frame libera el contenido del paquete
        if (av_interleaved_write_frame(this->context.getContext(), pkt) < 0) {
            throw std::runtime_error("Error al escribir en el archivo de salida");
        }
    }
}

OutputFormat::OutputFormat(FormatContext& context,
                           const std::string& filename) :
    context(context), avOutputFormat(nullptr), videoStream(nullptr),
    codecContext(nullptr), headerWritten(false), trailerWritten(false) {
    // Si el constructor lanza una excepción no se ejecuta el destructor,
    // hay que liberar a mano lo reservado hasta ese momento
    try {
        const std::string outputName = withExtension(filename);
        // Intenta deducir formato según extensión
        this->avOutputFormat = av_guess_format(NULL, outputName.c_str(), NULL);
        if (!this->avOutputFormat) {
            // Intenta usar el formato standard
            this->avOutputFormat = av_guess_format("mp4", NULL, NULL);
        }
        if (!this->avOutputFormat) {
            throw std::runtime_error("No se encontró formato de salida");
        }
        // h.264 es bastante popular, pero hay mejores. Si el contenedor no lo
        // soporta (por ejemplo webm) se usa el codec por defecto del mismo
        AVCodecID codecId = AV_CODEC_ID_H264;
        // (devuelve 0 si no lo soporta, negativo si no lo sabe)
        if (avformat_query_codec(this->avOutputFormat, codecId,
                                 FF_COMPLIANCE_NORMAL) == 0) {
            codecId = this->avOutputFormat->video_codec;
        }
        const AVCodec *codec = avcodec_find_encoder(codecId);
        if (!codec) {
            throw std::runtime_error("No se pudo instanciar codec");
        }
        codecContextInit(codec);
        openOutput(outputName);
    } catch (...) {
        release();
        throw;
    }
}

void OutputFormat::openOutput(const std::string& outputName) {
    AVFormatContext* formatContext = this->context.getContext();
    formatContext->oformat = this->avOutputFormat;

    // El stream es liberado junto con el formatContext
    this->videoStream = avformat_new_stream(formatContext, NULL);
    if (!this->videoStream) {
        throw std::runtime_error("No se pudo crear el stream de video");
    }
    this->videoStream->time_base = this->codecContext->time_base;
    this->videoStream->avg_frame_rate = this->codecContext->framerate;
    // Copia al stream los parámetros con los que se abrió el codec
    if (avcodec_parameters_from_context(this->videoStream->codecpar,
                                        this->codecContext) < 0) {
        throw std::runtime_error("No se pudieron copiar los parámetros del codec");
    }

    if (!(this->avOutputFormat->flags & AVFMT_NOFILE)) {
        if (avio_open(&formatContext->pb, outputName.c_str(), AVIO_FLAG_WRITE) < 0) {
            throw std::runtime_error("No se pudo abrir el archivo de salida");
        }
    }
    // El muxer puede cambiar el time_base del stream al escribir el header
    if (avformat_write_header(formatContext, NULL) < 0) {
        throw std::runtime_error("No se pudo iniciar header");
    }
    this->headerWritten = true;
}

void OutputFormat::writeData() {
    std::unique_ptr<AVFrame, FrameDeleter> frameOwner(av_frame_alloc());
    AVFrame *frame = frameOwner.get();
    if (!frame) {
        throw std::runtime_error("No se pudo reservar memoria para frame");
    }
    // Formato popular en h264, experimentar con otros.
    frame->format = this->codecContext->pix_fmt;
    frame->width  = this->codecContext->width;
    frame->height = this->codecContext->height;
    
    if (av_frame_get_buffer(frame, 0) < 0) {
        throw std::runtime_error("No se pudo reservar el buffer del frame");
    }
    std::unique_ptr<AVPacket, PacketDeleter> pktOwner(av_packet_alloc());
    AVPacket* pkt = pktOwner.get();
    if (!pkt) {
        throw std::runtime_error("No se pudo reservar memoria para paquete");
    }
    int pts = 0;
    for(int i = 0; i<50; i++) {
        // El encoder puede quedarse con una referencia al buffer del frame
        // anterior, hay que asegurarse de que se pueda escribir
        if (av_frame_make_writable(frame) < 0) {
            throw std::runtime_error("No se pudo escribir el frame");
        }
        drawFrame(frame, i);
        frame->pts = pts;
        pts++;
        /* encode the image */
        encode(frame, pkt);
    }
    // Vacía los paquetes que le quedan al encoder
    encode(NULL, pkt);
    // El trailer cierra el contenedor, sin él un mp4 no se puede reproducir
    this->trailerWritten = true;
    if (av_write_trailer(this->context.getContext()) < 0) {
        throw std::runtime_error("No se pudo escribir el trailer");
    }
}
 
void OutputFormat::drawFrame(AVFrame* frame, int i) {
    /**
     * YUV444/422 Format:
     * Size of data[0] is linesize[0] * AVFrame::height
     * Size of data[1] is linesize[1] * AVFrame::height
     * Size of data[2] is linesize[2] * AVFrame::height
     * 
     * YUV420 Format:
     * Size of data[0] is linesize[0] * AVFrame::height
     * Size of data[1] is linesize[1] * (AVFrame::height / 2)
     * Size of data[2] is linesize[2] * (AVFrame::height / 2)
     */
    /* Y */
    for(int y=0; y < frame->height; y++) {
        for(int x=0; x < frame->width; x++) {
            frame->data[0][y * frame->linesize[0] + x] = y + i * 3;
        }
    }
    
    // Cb y Cr se escriben en la mitad del buffer
    for(int y=0; y < frame->height / 2; y++) {
        for(int x=0; x < frame->width / 2; x++) {
            frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
            frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
        }
    }
}

void OutputFormat::codecContextInit(const AVCodec* codec){
    this->codecContext = avcodec_alloc_context3(codec);
    if (!this->codecContext) {
        throw std::runtime_error("No se pudo reservar memoria para el codec");
    }
    // La resolución debe ser múltiplo de 2
    this->codecContext->width = 352;
    this->codecContext->height = 288;
    this->codecContext->time_base = {1,25};
    this->codecContext->framerate = {25,1};
    this->codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    this->codecContext->gop_size = 10;
    // El perfil baseline no soporta B-frames
    this->codecContext->max_b_frames = 0;
    if (codec->id == AV_CODEC_ID_H264) {
        av_opt_set(this->codecContext->priv_data, "profile", "baseline", 0);
        av_opt_set(this->codecContext->priv_data, "preset", "slow", 0);
    }
    // Contenedores como mp4 guardan los headers del codec una sola vez en el
    // archivo, en lugar de repetirlos dentro del stream
    if (this->avOutputFormat->flags & AVFMT_GLOBALHEADER) {
        this->codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    if (avcodec_open2(this->codecContext, codec, NULL) < 0) {
        throw std::runtime_error("No se pudo abrir el codec");
    }
}

void OutputFormat::release() {
    AVFormatContext* formatContext = this->context.getContext();
    // Si hubo un error a mitad de la escritura, cierra el contenedor para
    // que lo escrito hasta el momento se pueda reproducir
    if (this->headerWritten && !this->trailerWritten) {
        av_write_trailer(formatContext);
        this->trailerWritten = true;
    }
    if (this->avOutputFormat && !(this->avOutputFormat->flags & AVFMT_NOFILE)) {
        avio_closep(&formatContext->pb);
    }
    avcodec_free_context(&this->codecContext);
}

OutputFormat::~OutputFormat() {
    release();
}
