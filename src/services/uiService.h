#ifndef UISERVICE_HEADER
#define UISERVICE_HEADER

#include <QObject>
#include <QString>
#include <QPixmap>
#include <QDate>
#include <QTime>
#include "../logic/event.h"
#include "../logic/datable.h"

class UIService : public QObject
{
    Q_OBJECT

public:
    explicit UIService(QObject *parent = nullptr);

    // Metodi di formattazione per gli eventi
    QString formatEventName(Event* event) const;
    QString formatEventNote(Event* event) const;
    
    // Formattazione date e ore
    QString formatDate(int date) const;
    QString formatTime(int hour, int minute = 0) const;  // Aggiunto parametro minute
    QString formatDuration(int minutes) const;
    QDate intToQDate(int date) const;
    int qDateToInt(const QDate& date) const;
    
    // Validazione date/ore
    bool isValidDate(int date) const;
    bool isValidHour(int hour) const;
    bool isValidMinute(int minute) const;  // Nuovo metodo
    
    // Gestione immagini con default
    QPixmap loadEventImage(const std::string& imagePath, const std::string& eventType) const;
    QString getDefaultImagePath(const std::string& eventType) const;

private:
    // Metodi helper per immagini
    QPixmap createImagePlaceholder(const std::string& originalPath) const;
    QString resolveImagePath(const QString& relativePath) const;
    QString createDebugInfo(const QString& originalPath, const QString& resolvedPath) const;
    
    // Path per immagini di default
    const QString DEFAULT_IMAGE_FOLDER = "../resources/img/default/";
    const QMap<QString, QString> DEFAULT_IMAGES = {
        {"Appointment", "appointment.png"},
        {"Deadline", "deadline.png"},
        {"Recursive", "recursive.png"},
        {"Reminder", "reminder.png"}
    };
};

#endif