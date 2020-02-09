//
// Created by camix on 12/06/19.
//

#ifndef FFMPEG_DEMO_CONSUMER_H
#define FFMPEG_DEMO_CONSUMER_H


#include "Thread.h"
#include "BlockingQueue.h"
#include "Output.h"
#include "FormatContext.h"

class Consumer : public Thread  {
private:
    BlockingQueue& producedFrames;
    FormatContext context;
    Output videoOutput;
    // You need it to perform scaling/conversion operations using.
    SwsContext* ctx;

public:
    /// first you need to Initialize libavformat and register all the muxers, demuxers and
    /// protocols with av_register_all();
    Consumer(BlockingQueue& producedImages, std::string& filename);

    ~Consumer();

    virtual void run() override;

};


#endif //FFMPEG_DEMO_CONSUMER_H
