#include "OutputFormat.h"
#include "FormatContext.h"
#include <cstdio>
#include <string>
#include <stdexcept>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                   FILE *outfile) {
    int ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        throw std::runtime_error("Error al enviar frame");
    }
    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            throw std::runtime_error("Error al codificar");
        }
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}

OutputFormat::OutputFormat(FormatContext& context,
                           const std::string& filename) : context(context) {
    this->frame = av_frame_alloc();
    if (!frame) {
        throw std::runtime_error("No se pudo reservar memoria para frame");
    }
    this->pkt = av_packet_alloc();
    // Intenta deducir formato según extensión
    this->avOutputFormat = av_guess_format(NULL, filename.c_str(), NULL);
    if (!this->avOutputFormat) {
        // Intenta usar el formato standard
        this->avOutputFormat = av_guess_format("mpeg", NULL, NULL);
    }
    if (!this->avOutputFormat) {
        throw std::runtime_error("No se encontró formato de salida");
    }
    // h.264 es bastante popular, pero hay mejores
    this->avOutputFormat->video_codec = AV_CODEC_ID_H264;
    AVCodec *codec = avcodec_find_encoder(this->avOutputFormat->video_codec);
    if (!codec) {
        throw std::runtime_error("No se pudo instanciar codec");
    }
    codecContextInit(codec);
    this->outputFile = fopen(filename.c_str(), "wb");
    initFrame();
}

void OutputFormat::close() {
    encode(this->codecContext, NULL, this->pkt, this->outputFile);
    /* add sequence end code to have a real MPEG file */
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    fwrite(endcode, 1, sizeof(endcode), this->outputFile);
}

void OutputFormat::initFrame() {
    this->frame->format = this->codecContext->pix_fmt;
    this->frame->width  = this->codecContext->width;
    this->frame->height = this->codecContext->height;
    
    av_frame_get_buffer(this->frame, 0);
    this->currentPts = 0;
}

void OutputFormat::writeFrame(const char* data, SwsContext* ctx ) {
    const u_int8_t* tmp = (const u_int8_t*) data;
    // El ancho del video x3 por la cantidad de bytes
    int width = 352 * 3;
    sws_scale(ctx, &tmp, &width, 0, frame->height, frame->data, frame->linesize);
    //drawFrame(frame, data);
    frame->pts = currentPts;
    currentPts++;
    /* encode the image */
    encode(this->codecContext, frame, pkt, this->outputFile);
}

void OutputFormat::codecContextInit(AVCodec* codec){
    this->codecContext = avcodec_alloc_context3(codec);
    // La resolución debe ser múltiplo de 2
    this->codecContext->width = 352;
    this->codecContext->height = 288;
    this->codecContext->time_base = {1,25};
    this->codecContext->framerate = {25,1};
    this->codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    this->codecContext->gop_size = 10;
    this->codecContext->max_b_frames = 2;
    if (codec->id == AV_CODEC_ID_H264) {
        this->codecContext->profile = FF_PROFILE_H264_BASELINE;
        av_opt_set(this->codecContext->priv_data, "preset", "slow", 0);
    }
    avcodec_open2(this->codecContext, codec, NULL);
}

OutputFormat::~OutputFormat() {
    avcodec_close(this->codecContext);
    avcodec_free_context(&this->codecContext);
    av_packet_free(&pkt);
    av_frame_free(&frame);
    fclose(this->outputFile);
}
