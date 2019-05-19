#ifndef __SDL_TEXTURE_H__
#define __SDL_TEXTURE_H__
#include <string>

class SDL_Texture;
class SDL_Renderer;
class SdlWindow;
class Area;

class SdlTexture {
public:
    /**
     * Crea un SDL_Texture, lanza una excepción si el filename es inválido
     **/
    SdlTexture(const std::string &filename, const SdlWindow& window);
    /**
     * Libera la memoria reservada por la textura
     **/
    ~SdlTexture();
    /**
     * Renderiza la textura cargada
     **/
    int render(const Area& src, const Area& dest) const;
    /**
     * Asigna como destino de render
     */
    int setAsTarget() const;
private:
    SDL_Texture* loadTexture(const std::string &filename);
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};

#endif
