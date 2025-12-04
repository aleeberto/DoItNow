#include "uiService.h"
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QPainter>
#include <QFont>
#include <QDebug>

UIService::UIService(QObject *parent) : QObject(parent) {}

QString UIService::formatEventName(Event* event) const {
    return event ? "<b>" + QString::fromStdString(event->getName()) + "</b>" : "";
}

QString UIService::formatEventNote(Event* event) const {
    return event ? QString::fromStdString(event->getNote()) : "";
}

// ==================== FORMATTAZIONE DATE ====================

QString UIService::formatDate(int date) const {
    if (!isValidDate(date)) {
        return "Data non valida";
    }
    
    QDate qDate = intToQDate(date);
    return qDate.toString("dd/MM/yyyy");
}

QString UIService::formatTime(int hour) const {
    if (!isValidHour(hour)) {
        return "Ora non valida";
    }
    
    return QString("%1:00").arg(hour, 2, 10, QChar('0'));
}

QString UIService::formatDuration(int minutes) const {
    if (minutes < 0) {
        return "Durata non valida";
    }
    
    if (minutes < 60) {
        return QString("%1 min").arg(minutes);
    } else {
        int hours = minutes / 60;
        int mins = minutes % 60;
        if (mins == 0) {
            return QString("%1h").arg(hours);
        } else {
            return QString("%1h %2min").arg(hours).arg(mins);
        }
    }
}

QDate UIService::intToQDate(int date) const {
    if (date < 10000000 || date > 99999999) {
        return QDate(); // Data non valida
    }
    
    int year = date / 10000;
    int month = (date % 10000) / 100;
    int day = date % 100;
    
    return QDate(year, month, day);
}

int UIService::qDateToInt(const QDate& date) const {
    if (!date.isValid()) {
        return 0;
    }
    
    return date.year() * 10000 + date.month() * 100 + date.day();
}

// ==================== VALIDAZIONE ====================

bool UIService::isValidDate(int date) const {
    QDate qDate = intToQDate(date);
    return qDate.isValid() && qDate.year() >= 2000 && qDate.year() <= 2100;
}

bool UIService::isValidHour(int hour) const {
    return hour >= 0 && hour <= 23;
}

// ==================== GESTIONE IMMAGINI ====================

QPixmap UIService::loadEventImage(const std::string& imagePath, const std::string& eventType) const {
    QString imageQPath = QString::fromStdString(imagePath);
    QPixmap pixmap;
    
    // Se il percorso è vuoto o non valido, usa l'immagine di default
    if (imagePath.empty() || imagePath == "default") {
        QString defaultPath = getDefaultImagePath(eventType);
        if (pixmap.load(defaultPath)) {
            return pixmap.scaled(120, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }
    
    // Prova a caricare l'immagine specificata
    QString resolvedPath = resolveImagePath(imageQPath);
    if (pixmap.load(resolvedPath)) {
        return pixmap.scaled(120, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    // Se fallisce, prova con l'immagine di default
    QString defaultPath = getDefaultImagePath(eventType);
    if (pixmap.load(defaultPath)) {
        qDebug() << "Caricata immagine di default per tipo:" << QString::fromStdString(eventType);
        return pixmap.scaled(120, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    // Se anche quella fallisce, crea un placeholder
    return createImagePlaceholder(imagePath);
}

QString UIService::getDefaultImagePath(const std::string& eventType) const {
    QString type = QString::fromStdString(eventType);
    
    if (DEFAULT_IMAGES.contains(type)) {
        QString relativePath = DEFAULT_IMAGE_FOLDER + DEFAULT_IMAGES[type];
        return resolveImagePath(relativePath);
    }
    
    // Se il tipo non è riconosciuto, usa un'immagine generica
    QString genericPath = DEFAULT_IMAGE_FOLDER + "event.png";
    return resolveImagePath(genericPath);
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