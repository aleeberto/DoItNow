#include "uiService.h"
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QPainter>
#include <QFont>
#include <QDebug>

UIService::UIService(QObject *parent) : QObject(parent) {}

QString UIService::formatMediaTitle(Media* media) const {
    return media ? "<b>" + QString::fromStdString(media->getTitolo()) + "</b>" : "";
}

QString UIService::formatMediaYear(Media* media) const {
    return media ? "Anno: " + QString::number(media->getAnno()) : "";
}

QPixmap UIService::loadMediaImage(const std::string& imagePath) const {
    QString imageQPath = QString::fromStdString(imagePath);
    QPixmap pixmap;
    
    QString resolvedPath = resolveImagePath(imageQPath);
    
    if (pixmap.load(resolvedPath)) {
        return pixmap.scaled(120, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    return createImagePlaceholder(imagePath);
}

QPixmap UIService::createImagePlaceholder(const std::string& originalPath) const {
    QPixmap pixmap(120, 180);
    pixmap.fill(QColor(240, 240, 240));
    
    QPainter painter(&pixmap);
    painter.setPen(Qt::darkGray);
    painter.setFont(QFont("Arial", 8));
    
    QString imageQPath = QString::fromStdString(originalPath);
    QString resolvedPath = resolveImagePath(imageQPath);
    QString debugInfo = createDebugInfo(imageQPath, resolvedPath);
    
    painter.drawText(pixmap.rect().adjusted(5, 5, -5, -5), 
                    Qt::AlignCenter | Qt::TextWordWrap, 
                    debugInfo);
    
    qDebug() << "Errore caricamento immagine:" << debugInfo;
    
    return pixmap;
}

QString UIService::resolveImagePath(const QString& relativePath) const {
    QFileInfo fileInfo(relativePath);
    if (fileInfo.isRelative()) {
        QString appDir = QCoreApplication::applicationDirPath();
        return QDir::cleanPath(appDir + QDir::separator() + relativePath);
    }
    return relativePath;
}

QString UIService::createDebugInfo(const QString& originalPath, const QString& resolvedPath) const {
    return QString("Impossibile caricare:\n%1\nPercorso risolto:\n%2")
           .arg(originalPath)
           .arg(resolvedPath);
}