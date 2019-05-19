#ifndef __SDL_EXCEPTION_H__
#define __SDL_EXCEPTION_H__
#include <string>
#include <exception>

class SdlException : public std::exception {
public:
    SdlException(const char* description, const char* sdlError);
    const char* what() const noexcept;
private:
    std::string description;
};

#endif
