//
// Created by camix on 12/06/19.
//

#ifndef FFMPEG_DEMO_THREAD_H
#define FFMPEG_DEMO_THREAD_H


#include <thread>

class Thread {
private:
    std::thread thread;

public:
    Thread();
    void start();
    void join();
    virtual void run() = 0;
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;
    virtual ~Thread();
    Thread(Thread&& other);
};


#endif //FFMPEG_DEMO_THREAD_H
