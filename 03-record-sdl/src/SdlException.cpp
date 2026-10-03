#include "SdlException.h"
#include <string>

SdlException::SdlException(const char* description, const char* sdlError)
    : std::exception(), description(description) {
    this->description.append("\n SDL_ERROR: ").append(sdlError);
}

const char* SdlException::what() const noexcept{
    return this->description.c_str();
}
