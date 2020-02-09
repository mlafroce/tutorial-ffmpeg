//
// Created by camix on 10/06/19.
//

#ifndef FFMPEG_DEMO_SWSCONTEXT_H
#define FFMPEG_DEMO_SWSCONTEXT_H


#define RRP_SWS_CONTEXT_EXC "There was an error while writing the frame\n"

#include <string>
#include <vector>
#include "FormatContext.h"
#include "Output.h"
#include "SdlWindow.h"
#include "BlockingQueue.h"
#include "Consumer.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


class SwsContext {
private:
    std::vector<char> dataBuffer;
    BlockingQueue& producedFrames;

public:
    SwsContext(BlockingQueue& producedFrames);
    ~SwsContext();
    void write(SdlWindow& window);
};

class SwsContextRendererReadPixelsException : public std::exception {
public:
    SwsContextRendererReadPixelsException() = default;
    virtual const char *what() const throw() {
        return RRP_SWS_CONTEXT_EXC;
    }
};


#endif //FFMPEG_DEMO_SWSCONTEXT_H
