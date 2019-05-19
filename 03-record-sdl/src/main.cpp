#include <SDL2/SDL.h>
#include <exception>
#include <iostream>
#include <vector>
#include "SdlWindow.h"
#include "SdlTexture.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include "FormatContext.h"
#include "OutputFormat.h"

const int BUFFER_WIDTH = 352, BUFFER_HEIGHT = 288;

void handleSDLEvent(int &x, int &y, bool& running);

int main(int argc, char** argv){
    if(argc < 2) {
        printf("Please provide a movie file\n");
        return -1;
    }
    try {
        av_register_all();
        FormatContext context;
        OutputFormat videoOutput(context, argv[1]);
        SdlWindow window(800, 600);
        window.fill();
        // Usar factory
        SdlTexture catTexture("cat.gif", window);
        Area srcArea(0, 0, 300, 300);
        bool running = true;
        int x = 0;
        int y = 0;
        // Textura sobre la que voy a renderizar lo que quiero grabar.
        SDL_Texture* videoTexture = SDL_CreateTexture(window.getRenderer(),
            SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, BUFFER_WIDTH, BUFFER_HEIGHT);
        // Contexto para escalar archivos.
        SwsContext * ctx = sws_getContext(BUFFER_WIDTH, BUFFER_HEIGHT,
                                  AV_PIX_FMT_RGB24, BUFFER_WIDTH, BUFFER_HEIGHT,
                                  AV_PIX_FMT_YUV420P, 0, 0, 0, 0);
        // Este buffer tiene el tamaño de la sección de SDL que quiero leer, multiplico
        // x3 por la cantidad de bytes (8R,8G,8B)
        // A sws parece que no le gusta este tamaño
        std::vector<char> dataBuffer(BUFFER_WIDTH*BUFFER_HEIGHT*3);
        while (running) {
            // Muevo textura con flechas direccionales
            handleSDLEvent(x, y, running);
            Area destArea(x, y, 160, 160);
            // Render sobre ventana
            SDL_SetRenderTarget(window.getRenderer(), NULL);
            window.fill(); // Repinto el fondo gris
            catTexture.render(srcArea, destArea);
            // Render sobre la textura que quiero guardar
            SDL_SetRenderTarget(window.getRenderer(), videoTexture);
            window.fill();
            catTexture.render(srcArea, destArea);
            // Efectivamente renderiza
            window.render();
            // Obtengo los bytes de la textura en el buffer
            int res = SDL_RenderReadPixels(window.getRenderer(), NULL, SDL_PIXELFORMAT_RGB24, dataBuffer.data(), BUFFER_WIDTH * 3);
            if (res) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "RendererReadPixels error", SDL_GetError(), NULL);
                break;
            }
            videoOutput.writeFrame(dataBuffer.data(), ctx);
        }
        videoOutput.close();
        // Libero escalador
        sws_freeContext(ctx);
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

void handleSDLEvent(int &x, int &y, bool& running) {
    SDL_Event event;
    SDL_WaitEvent(&event);
    switch(event.type) {
        case SDL_KEYDOWN: {
                SDL_KeyboardEvent& keyEvent = (SDL_KeyboardEvent&) event;
                switch (keyEvent.keysym.sym) {
                    case SDLK_LEFT:
                        x -= 1;
                        break;
                    case SDLK_RIGHT:
                        x += 1;
                        break;
                    case SDLK_UP:
                        y -= 1;
                        break;
                    case SDLK_DOWN:
                        y += 1;
                        break;
                    }
            } // Fin KEY_DOWN
            break;
        case SDL_MOUSEMOTION:
            std::cout << "Oh! Mouse" << std::endl;
            break;
        case SDL_QUIT:
            std::cout << "Quit :(" << std::endl;
            running = false;
            break;
    }
}