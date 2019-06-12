//
// Created by camix on 12/06/19.
//

#include "Consumer.h"


Consumer::Consumer(BlockingQueue producedFrames, Output& outputVideo, SwsContext* ctx, int i) :
        producedFrames(producedFrames),
        outputVideo(outputVideo),
        ctx(ctx),
        i(i) {
}

void Consumer::run() {
    try {
        while (true) {
            std::vector<char> frame = this->producedFrames.pop();
            outputVideo.writeFrame(frame.data(), ctx);
        }
    }
    catch (BlockingQueueDoneException& e) {
        return;
    }
}
