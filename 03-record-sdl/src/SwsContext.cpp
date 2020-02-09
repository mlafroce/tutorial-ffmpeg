//
// Created by camix on 10/06/19.
//

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_messagebox.h>
#include "SwsContext.h"
#include "Constants.h"



SwsContext::SwsContext(BlockingQueue& producedFrames) :
    // Este buffer tiene el tamaño de la sección de SDL que quiero leer, multiplico
    // x3 por la cantidad de bytes (8R,8G,8B)
    // A sws parece que no le gusta este tamaño
    dataBuffer(BUFFER_WIDTH*BUFFER_HEIGHT*3),
    producedFrames(producedFrames)
    {}

SwsContext::~SwsContext() = default;



void SwsContext::write(SdlWindow& window) {
    // Obtengo los bytes de la textura en el buffer
    int res = SDL_RenderReadPixels(window.getRenderer(), NULL, SDL_PIXELFORMAT_RGB24, dataBuffer.data(), BUFFER_WIDTH * 3);
    if (res) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "RendererReadPixels error", SDL_GetError(), NULL);
        throw SwsContextRendererReadPixelsException();
    }
    //operacion lenta que pasó a otro thread:
        //videoOutput.writeFrame(dataBuffer.data(), ctx);
    producedFrames.push(dataBuffer);
}
