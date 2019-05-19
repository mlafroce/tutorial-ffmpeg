#ifndef FORMATCONTEXT_H
#define FORMATCONTEXT_H
#include <string>

class AVFormatContext;
class FormatContext {
public:
    // Ctor
    FormatContext();
    // Dtor
    ~FormatContext();
    // Abre un archivo de video e imprime su informacion de formato
    void dumpFormat(const std::string& filename);
private:
    AVFormatContext *pFormatCtx;
};
#endif
