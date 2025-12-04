#ifndef UISERVICE_HEADER
#define UISERVICE_HEADER

#include <QObject>
#include <QString>
#include <QPixmap>
#include "../logic/media.h"

class UIService : public QObject
{
    Q_OBJECT

public:
    explicit UIService(QObject *parent = nullptr);

    // Metodi di formattazione per i media
    QString formatMediaTitle(Media* media) const;
    QString formatMediaYear(Media* media) const;
    
    // Gestione immagini
    QPixmap loadMediaImage(const std::string& imagePath) const;

private:
    // Metodi helper
    QPixmap createImagePlaceholder(const std::string& originalPath) const;
    QString resolveImagePath(const QString& relativePath) const;
    QString createDebugInfo(const QString& originalPath, const QString& resolvedPath) const;
};

#endif