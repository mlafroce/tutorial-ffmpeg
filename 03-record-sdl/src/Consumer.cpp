//
// Created by camix on 12/06/19.
//
extern "C" {
#include <libswscale/swscale.h>
}
#include "Consumer.h"
#include "Constants.h"

Consumer::Consumer(BlockingQueue& producedFrames, std::string& filename) :
        producedFrames(producedFrames),
    videoOutput(context, filename),
    ctx(sws_getContext(BUFFER_WIDTH, BUFFER_HEIGHT,
                   AV_PIX_FMT_RGB24, BUFFER_WIDTH, BUFFER_HEIGHT,
                   AV_PIX_FMT_YUV420P, 0, 0, 0, 0))  {
}

void Consumer::run() {
    try {
        while (true) {
            std::vector<char> frame = this->producedFrames.pop();
            videoOutput.writeFrame(frame.data(), ctx);
        }
    }
    catch (BlockingQueueDoneException& e) {
        return;
    }
}

Consumer::~Consumer() {
    videoOutput.close();
    sws_freeContext(ctx);
}