#ifndef JSONTYPEVISITOR_HEADER
#define JSONTYPEVISITOR_HEADER

#include <QString>
#include "../logic/mediaVisitor.h"

class JsonTypeVisitor : public MediaVisitor {
private:
    QString mediaType;
    
public:
    JsonTypeVisitor() = default;
    
    void visit(Film* film) override;
    void visit(SerieTv* serieTv) override;
    void visit(Anime* anime) override;
    void visit(Libro* libro) override;
    void visit(Manga* manga) override;
    void visit(Cd* cd) override;
    
    QString getMediaType() const;
    void reset();
};

#endif
