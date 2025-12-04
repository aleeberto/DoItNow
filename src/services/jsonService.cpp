#include "jsonService.h"
#include "../logic/film.h"
#include "../logic/serieTv.h"
#include "../logic/anime.h"
#include "../logic/libro.h"
#include "../logic/manga.h"
#include "../logic/cd.h"

JsonService::JsonService(QObject *parent) : QObject(parent) {
    initializeFactories();
}

void JsonService::initializeFactories() {
    // Registrazione delle factory per ogni tipo di media
    mediaFactories["Film"] = [](const QJsonObject& json) -> std::unique_ptr<Media> {
        std::string titolo = json["titolo"].toString().toStdString();
        int anno = json["anno"].toInt();
        std::string immagine = json["immagine"].toString().toStdString();
        
        auto film = std::make_unique<Film>(titolo, anno, immagine, "", "", 0);
        film->fromJsonSpecific(json);
        return film;
    };
    
    mediaFactories["Serie Tv"] = [](const QJsonObject& json) -> std::unique_ptr<Media> {
        std::string titolo = json["titolo"].toString().toStdString();
        int anno = json["anno"].toInt();
        std::string immagine = json["immagine"].toString().toStdString();
        
        auto serie = std::make_unique<SerieTv>(titolo, anno, immagine, 0, 0, 0, false, "", "");
        serie->fromJsonSpecific(json);
        return serie;
    };
    
    mediaFactories["Anime"] = [](const QJsonObject& json) -> std::unique_ptr<Media> {
        std::string titolo = json["titolo"].toString().toStdString();
        int anno = json["anno"].toInt();
        std::string immagine = json["immagine"].toString().toStdString();
        
        auto anime = std::make_unique<Anime>(titolo, anno, immagine, 0, 0, 0, false, "", "");
        anime->fromJsonSpecific(json);
        return anime;
    };
    
    mediaFactories["Libro"] = [](const QJsonObject& json) -> std::unique_ptr<Media> {
        std::string titolo = json["titolo"].toString().toStdString();
        int anno = json["anno"].toInt();
        std::string immagine = json["immagine"].toString().toStdString();
        
        auto libro = std::make_unique<Libro>(titolo, anno, immagine, "", 0, "");
        libro->fromJsonSpecific(json);
        return libro;
    };
    
    mediaFactories["Manga"] = [](const QJsonObject& json) -> std::unique_ptr<Media> {
        std::string titolo = json["titolo"].toString().toStdString();
        int anno = json["anno"].toInt();
        std::string immagine = json["immagine"].toString().toStdString();
        
        auto manga = std::make_unique<Manga>(titolo, anno, immagine, "", "", 0, false);
        manga->fromJsonSpecific(json);
        return manga;
    };
    
    mediaFactories["Cd"] = [](const QJsonObject& json) -> std::unique_ptr<Media> {
        std::string titolo = json["titolo"].toString().toStdString();
        int anno = json["anno"].toInt();
        std::string immagine = json["immagine"].toString().toStdString();
        
        auto cd = std::make_unique<Cd>(titolo, anno, immagine, "", 0, 0);
        cd->fromJsonSpecific(json);
        return cd;
    };
}

bool JsonService::loadFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Impossibile aprire il file:" << filePath;
        return false;
    }
    
    QByteArray jsonData = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        qWarning() << "Formato JSON non valido";
        return false;
    }
    
    mediaArray = doc.object()["media"].toArray();
    return true;
}

bool JsonService::saveToFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Impossibile salvare il file:" << filePath;
        return false;
    }
    
    QJsonObject rootObject;
    rootObject["media"] = mediaArray;
    
    QJsonDocument doc(rootObject);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

QVector<Media*> JsonService::getAllMedia() const {
    QVector<Media*> result;
    
    for (const QJsonValue &value : mediaArray) {
        auto media = createMediaFromJson(value.toObject());
        if (media) {
            result.append(media.release());
        }
    }
    
    return result;
}

void JsonService::addMedia(Media *media) {
    if (!media) return;
    mediaArray.append(mediaToJson(media));
}

void JsonService::clearAll() {
    mediaArray = QJsonArray();
}

std::unique_ptr<Media> JsonService::createMediaFromJson(const QJsonObject &jsonObj) const {
    QString type = jsonObj["type"].toString();
    std::string typeStr = type.toStdString();
    
    auto factoryIt = mediaFactories.find(typeStr);
    if (factoryIt != mediaFactories.end()) {
        return factoryIt->second(jsonObj);
    }
    
    qWarning() << "Tipo di media sconosciuto:" << type;
    return nullptr;
}

QJsonObject JsonService::mediaToJson(Media *media) {
    if (!media) return QJsonObject();
    
    QJsonObject jsonObj;
    
    // Usa il visitor per ottenere il tipo
    typeVisitor.reset();
    media->accept(&typeVisitor);
    
    jsonObj["titolo"] = QString::fromStdString(media->getTitolo());
    jsonObj["anno"] = media->getAnno();
    jsonObj["immagine"] = QString::fromStdString(media->getImmagine());
    
    QJsonObject specificData = media->toJsonSpecific();
    for (auto it = specificData.begin(); it != specificData.end(); ++it) {
        jsonObj[it.key()] = it.value();
    }
    
    return jsonObj;
}