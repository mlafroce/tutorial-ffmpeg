#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include "SdlException.h"
#include "SdlWindow.h"
#include <iostream>


SdlWindow::SdlWindow(int width, int height) :
        width(width), height(height), window(nullptr), renderer(nullptr) {
    int errCode = SDL_Init(SDL_INIT_VIDEO);
    if (errCode) {
        throw SdlException("Error en la inicialización", SDL_GetError());
    }
    // El tercer parámetro son flags de ventana (SDL_WindowFlags), no de renderer
    errCode = SDL_CreateWindowAndRenderer(
        width, height, 0,
        &this->window, &this->renderer);
    if (errCode) {
        // Copio el mensaje antes de SDL_Quit, el destructor no se ejecuta
        SdlException exception("Error al crear ventana", SDL_GetError());
        SDL_Quit();
        throw exception;
    }
}


SdlWindow::~SdlWindow() {
    std::cout << "Destruyendo" << std::endl;
    if (this->renderer) {
        SDL_DestroyRenderer(this->renderer);
        this->renderer = nullptr;
    }

    if (this->window) {
        SDL_DestroyWindow(this->window);
        this->window = nullptr;
    }
    SDL_Quit();
}

void SdlWindow::fill(int r, int g, int b, int alpha) {
    SDL_SetRenderDrawColor(this->renderer,
                           r, g, b, alpha);
    SDL_RenderClear(this->renderer);
}

void SdlWindow::fill() {
    this->fill(0x33,0x33,0x33,0xFF);
}

void SdlWindow::render() {
    SDL_RenderPresent(this->renderer);
}

SDL_Renderer* SdlWindow::getRenderer() const {
    return this->renderer;
}
