#ifndef JSONSERVICE_HEADER
#define JSONSERVICE_HEADER

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <memory>
#include <functional>
#include <unordered_map>
#include "../logic/event.h"
#include "jsonTypeVisitor.h"

class JsonService : public QObject
{
    Q_OBJECT
public:
    explicit JsonService(QObject *parent = nullptr);
    
    // Operazioni file
    bool loadFromFile(const QString &filePath);
    bool saveToFile(const QString &filePath);
    
    // Operazioni sugli eventi
    QVector<Event*> getAllEvents() const;
    void addEvent(Event *event);
    void clearAll();
    
private:
    QJsonArray eventArray;
    JsonTypeVisitor typeVisitor;
    
    // Factory map per la creazione
    std::unordered_map<std::string, std::function<std::unique_ptr<Event>(const QJsonObject&)>> eventFactories;
    
    // Metodi helper 
    std::unique_ptr<Event> createEventFromJson(const QJsonObject &jsonObj) const;
    QJsonObject eventToJson(Event *event);
    void initializeFactories();
};

#endif