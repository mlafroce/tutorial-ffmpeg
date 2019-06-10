//
// Created by camix on 10/06/19.
//

#ifndef FFMPEG_DEMO_CODECCONTEXT_H
#define FFMPEG_DEMO_CODECCONTEXT_H

extern "C" {
#include <libavcodec/avcodec.h>
}


class CodecContext {
private:
    AVCodecContext* codecContext;

public:
    explicit CodecContext(AVCodec *codec);

    ~CodecContext();

    AVCodecContext *get();

    int getHeight();

    int getWidth();

    int getPixFmt();
};


#endif //FFMPEG_DEMO_CODECCONTEXT_H
