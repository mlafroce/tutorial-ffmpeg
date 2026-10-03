#ifndef __SDL_WINDOW_H__
#define __SDL_WINDOW_H__
#include "Area.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class SdlWindow {
public:
    /**
     * Ctor standalone
     */
    SdlWindow(int width, int height);
    ~SdlWindow();
    // No copiable, es dueño de la ventana y el renderer
    SdlWindow(const SdlWindow&) = delete;
    SdlWindow& operator=(const SdlWindow&) = delete;
    void fill();
    void fill(int r, int g, int b, int alpha);
    void render();
    SDL_Renderer* getRenderer() const;
private:
    int width;
    int height;
    SDL_Window* window;
    SDL_Renderer* renderer;
};

#endif
