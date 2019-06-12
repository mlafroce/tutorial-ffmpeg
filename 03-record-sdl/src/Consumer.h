//
// Created by camix on 12/06/19.
//

#ifndef FFMPEG_DEMO_CONSUMER_H
#define FFMPEG_DEMO_CONSUMER_H


#include "Thread.h"
#include "BlockingQueue.h"
#include "Output.h"

class Consumer : public Thread  {
private:
    BlockingQueue& producedFrames;
    Output& outputVideo;
    SwsContext* ctx;
    int i;

public:
    //Recibe una cola bloqueante
    Consumer(BlockingQueue producedImages, Output& outputVideo, SwsContext* ctx, int i);

    //Espera la notificación de que hay brainfucks para ejecutar
    //y los ejecuta.
    virtual void run() override;
};


#endif //FFMPEG_DEMO_CONSUMER_H
