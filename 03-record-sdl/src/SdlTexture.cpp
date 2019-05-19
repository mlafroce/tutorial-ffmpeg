#include "SdlTexture.h"
#include <SDL2/SDL_image.h>
#include <string>
#include "SdlWindow.h"
#include "SdlException.h"

SdlTexture::SdlTexture(const std::string &filename, const SdlWindow& window)
    : renderer(window.getRenderer()) {
    this->texture = loadTexture(filename);
}

SdlTexture::~SdlTexture() {
    SDL_DestroyTexture(this->texture);
}

SDL_Texture* SdlTexture::loadTexture(const std::string &filename) {
    SDL_Texture* texture = IMG_LoadTexture(this->renderer,
                                           filename.c_str());
    if (!texture) {
        throw SdlException("Error al cargar la textura", SDL_GetError());
    }
    return texture;
}

int SdlTexture::render(const Area& src, const Area& dest) const {
    SDL_Rect sdlSrc = {
            src.getX(), src.getY(),
            src.getWidth(), src.getHeight()
    };
    SDL_Rect sdlDest = {
            dest.getX(), dest.getY(),
            dest.getWidth(), dest.getHeight()
    };

    return SDL_RenderCopy(this->renderer, this->texture, &sdlSrc, &sdlDest);
}

int SdlTexture::setAsTarget() const {
    return SDL_SetRenderTarget(this->renderer, this->texture);
}
