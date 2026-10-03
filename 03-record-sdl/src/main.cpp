#include <SDL2/SDL.h>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "SdlWindow.h"
#include "SdlTexture.h"
#include "SdlException.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include "FormatContext.h"
#include "OutputFormat.h"

const int BUFFER_WIDTH = 352, BUFFER_HEIGHT = 288;
// Bytes extra al final del buffer que se le pasa a sws_scale
const int SWS_PADDING = 64;

void handleSDLEvent(int &x, int &y, bool& running);

// Devuelve la ruta de un asset ubicado en la carpeta del ejecutable
static std::string assetPath(const std::string& name) {
    char* basePath = SDL_GetBasePath();
    if (!basePath) {
        return name;
    }
    std::string path = std::string(basePath) + name;
    SDL_free(basePath);
    return path;
}

int main(int argc, char** argv){
    if(argc < 2) {
        std::cerr << "Please provide a movie file" << std::endl;
        return 1;
    }
    try {
        FormatContext context;
        OutputFormat videoOutput(context, argv[1]);
        SdlWindow window(800, 600);
        window.fill();
        // Usar factory
        SdlTexture catTexture(assetPath("cat.gif"), window);
        Area srcArea(0, 0, 300, 300);
        bool running = true;
        int x = 0;
        int y = 0;
        // Textura sobre la que voy a renderizar lo que quiero grabar.
        std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> videoTexture(
            SDL_CreateTexture(window.getRenderer(), SDL_PIXELFORMAT_RGB24,
                SDL_TEXTUREACCESS_TARGET, BUFFER_WIDTH, BUFFER_HEIGHT),
            SDL_DestroyTexture);
        if (!videoTexture) {
            throw SdlException("Error al crear la textura de video", SDL_GetError());
        }
        // Contexto para escalar archivos.
        std::unique_ptr<SwsContext, void(*)(SwsContext*)> ctx(
            sws_getContext(BUFFER_WIDTH, BUFFER_HEIGHT,
                           AV_PIX_FMT_RGB24, BUFFER_WIDTH, BUFFER_HEIGHT,
                           AV_PIX_FMT_YUV420P, 0, 0, 0, 0),
            sws_freeContext);
        if (!ctx) {
            throw std::runtime_error("No se pudo crear el contexto de escalado");
        }
        // Este buffer tiene el tamaño de la sección de SDL que quiero leer, multiplico
        // x3 por la cantidad de bytes (8R,8G,8B)
        // sws lee con instrucciones SIMD de a 16 bytes y se pasa del final del
        // buffer si tiene el tamaño justo, por eso se agrega un padding
        std::vector<char> dataBuffer(BUFFER_WIDTH*BUFFER_HEIGHT*3 + SWS_PADDING);
        while (running) {
            // Muevo textura con flechas direccionales
            handleSDLEvent(x, y, running);
            Area destArea(x, y, 160, 160);
            // Render sobre la textura que quiero guardar
            if (SDL_SetRenderTarget(window.getRenderer(), videoTexture.get())) {
                // Si fallara, se leería la ventana entera sobre un buffer más chico
                throw SdlException("Error al asignar destino de render", SDL_GetError());
            }
            window.fill();
            catTexture.render(srcArea, destArea);
            // Obtengo los bytes de la textura en el buffer
            int res = SDL_RenderReadPixels(window.getRenderer(), NULL, SDL_PIXELFORMAT_RGB24, dataBuffer.data(), BUFFER_WIDTH * 3);
            // Render sobre ventana
            SDL_SetRenderTarget(window.getRenderer(), NULL);
            window.fill(); // Repinto el fondo gris
            catTexture.render(srcArea, destArea);
            // Efectivamente renderiza
            window.render();
            if (res) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "RendererReadPixels error", SDL_GetError(), NULL);
                break;
            }
            videoOutput.writeFrame(dataBuffer.data(), ctx.get());
        }
        videoOutput.close();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

void handleSDLEvent(int &x, int &y, bool& running) {
    SDL_Event event;
    if (!SDL_WaitEvent(&event)) {
        throw SdlException("Error al esperar evento", SDL_GetError());
    }
    switch(event.type) {
        case SDL_KEYDOWN: {
                SDL_KeyboardEvent& keyEvent = event.key;
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
