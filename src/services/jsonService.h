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
#include "../logic/media.h"
#include "jsonTypeVisitor.h"

class JsonService : public QObject
{
    Q_OBJECT
public:
    explicit JsonService(QObject *parent = nullptr);
    
    // Operazioni file
    bool loadFromFile(const QString &filePath);
    bool saveToFile(const QString &filePath);
    
    // Operazioni sui media
    QVector<Media*> getAllMedia() const;
    void addMedia(Media *media);
    void clearAll();
    
private:
    QJsonArray mediaArray;
    JsonTypeVisitor typeVisitor;
    
    // Factory map per la creazione
    std::unordered_map<std::string, std::function<std::unique_ptr<Media>(const QJsonObject&)>> mediaFactories;
    
    // Metodi helper 
    std::unique_ptr<Media> createMediaFromJson(const QJsonObject &jsonObj) const;
    QJsonObject mediaToJson(Media *media);
    void initializeFactories();
};

#endif