#ifndef FORMATCONTEXT_H
#define FORMATCONTEXT_H

struct AVFormatContext;
class FormatContext {
public:
    // Ctor
    FormatContext();
    // Dtor
    ~FormatContext();
    // No copiable, es dueño del contexto
    FormatContext(const FormatContext&) = delete;
    FormatContext& operator=(const FormatContext&) = delete;
    AVFormatContext* getContext() const;
private:
    AVFormatContext *pFormatCtx;
};
#endif
