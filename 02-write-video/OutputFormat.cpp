#include "OutputFormat.h"
#include "FormatContext.h"
#include <cstdio>
#include <string>
#include <stdexcept>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                   FILE *outfile)
{
    /* send the frame to the encoder */
    int ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        throw std::runtime_error("Error al enviar frame");
    }
    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            throw std::runtime_error("Error al codificar");
        }
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}

OutputFormat::OutputFormat(FormatContext& context,
                           const std::string& filename) : context(context) {
    this->outputFile = fopen(filename.c_str(), "wb");
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
    AVCodec *codec = avcodec_find_encoder(avOutputFormat->video_codec);
    //AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        throw std::runtime_error("No se pudo instancia codec");
    }
    this->codecContext = avcodec_alloc_context3(codec);
    this->codecContext->bit_rate = 400000;
    // La resolución debe ser múltiplo de 2
    this->codecContext->width = 352;
    this->codecContext->height = 288;
    this->codecContext->time_base = {1,25};
    this->codecContext->framerate = {25,1};
    this->codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    this->codecContext->gop_size = 10;
    this->codecContext->max_b_frames = 1;
    if (codec->id == AV_CODEC_ID_H264) {
        this->codecContext->profile = FF_PROFILE_H264_BASELINE;
        av_opt_set(this->codecContext->priv_data, "preset", "slow", 0);
    }
    
    avcodec_open2(this->codecContext, codec, NULL);
    this->video_avstream = avformat_new_stream(context.getContext(), codec);
}

void OutputFormat::writeData() {
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        throw std::runtime_error("No se pudo reservar memoria para frame");
    }
    // Formato popular en h264, experimentar con otros.
    frame->format = this->codecContext->pix_fmt;
    frame->width  = this->codecContext->width;
    frame->height = this->codecContext->height;
    
    av_frame_get_buffer(frame, 32);
    //avformat_write_header(context.getContext(), NULL);
    
    AVPacket* pkt = av_packet_alloc();
    for(int i = 0; i<25; i++) {
        /* Y */
        for(int y=0; y < frame->height; y++) {
            for(int x=0; x < frame->width; x++) {
                frame->data[0][y * frame->linesize[0] + x] = y + i * 3;
            }
        }
        
        // Cb y Ck se escriben en la mitad del buffer
        for(int y=0; y < frame->height / 2; y++) {
            for(int x=0; x < frame->width / 2; x++) {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }
        frame->pts = i;

        /* encode the image */
        encode(this->codecContext, frame, pkt, this->outputFile);
    }
    encode(this->codecContext, NULL, pkt, this->outputFile);
    /* add sequence end code to have a real MPEG file */
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    fwrite(endcode, 1, sizeof(endcode), this->outputFile);
    fclose(this->outputFile);
    av_packet_free(&pkt);
    av_frame_free(&frame);
}

OutputFormat::~OutputFormat() {
    avcodec_close(this->codecContext);
    avcodec_free_context(&this->codecContext);
}
