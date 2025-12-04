#include "jsonTypeVisitor.h"
#include "../logic/film.h"
#include "../logic/serieTv.h"
#include "../logic/anime.h"
#include "../logic/libro.h"
#include "../logic/manga.h"
#include "../logic/cd.h"

void JsonTypeVisitor::visit(Film* film) {
    Q_UNUSED(film)
    mediaType = "Film";
}

void JsonTypeVisitor::visit(SerieTv* serieTv) {
    Q_UNUSED(serieTv)
    mediaType = "Serie Tv";
}

void JsonTypeVisitor::visit(Anime* anime) {
    Q_UNUSED(anime)
    mediaType = "Anime";
}

void JsonTypeVisitor::visit(Libro* libro) {
    Q_UNUSED(libro)
    mediaType = "Libro";
}

void JsonTypeVisitor::visit(Manga* manga) {
    Q_UNUSED(manga)
    mediaType = "Manga";
}

void JsonTypeVisitor::visit(Cd* cd) {
    Q_UNUSED(cd)
    mediaType = "Cd";
}

QString JsonTypeVisitor::getMediaType() const {
    return mediaType;
}

void JsonTypeVisitor::reset() {
    mediaType.clear();
}