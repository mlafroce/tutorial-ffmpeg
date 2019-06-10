//
// Created by camix on 10/06/19.
//

#ifndef FFMPEG_DEMO_SWSCONTEXT_H
#define FFMPEG_DEMO_SWSCONTEXT_H


#define RRP_SWS_CONTEXT_EXC std::__cxx11::string()

#include <string>
#include <vector>
#include "FormatContext.h"
#include "Output.h"
#include "SdlWindow.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


class SwsContextException : public std::exception {
protected:
    std::string message;
public:
    SwsContextException() = default;
    virtual const char *what() const throw() {
        return this->message.c_str();
    }
};

class SwsContextRendererReadPixelsException : public SwsContextException {
public:
    SwsContextRendererReadPixelsException() {
        message = RRP_SWS_CONTEXT_EXC;
    }
};

// RAII wrapper for struct SwsContext
class SwsContext {
private:
    FormatContext context;
    Output videoOutput;
    // You need it to perform scaling/conversion operations using.
    SwsContext* ctx;
    std::vector<char> dataBuffer;

public:
    explicit SwsContext(std::string filename);
    ~SwsContext();
    void write(SdlWindow& window);
};


#endif //FFMPEG_DEMO_SWSCONTEXT_H
