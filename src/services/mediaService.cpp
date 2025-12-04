#include "mediaService.h"
#include <QMessageBox>
#include "../logic/film.h"
#include "../logic/serieTv.h"
#include "../logic/anime.h"
#include "../logic/libro.h"
#include "../logic/manga.h"
#include "../logic/cd.h"

// Definizione costanti statiche (case insensitive)
const QStringList MediaService::TRUE_VALUES = {"true", "1", "si", "yes"};
const QStringList MediaService::FALSE_VALUES = {"false", "0", "no"};

// Definizione configurazioni di validazione
const QMap<QString, MediaService::ValidationConfig> MediaService::validationConfigs = {
    {"Film", {MIN_FILM_FIELDS, 
             {{2, "Anno"}, {5, "Durata"}}, 
             {{5, "Durata"}}, 
             {}}},
    {"Serie Tv", {MIN_SERIES_FIELDS,
                 {{2, "Anno"}, {3, "Numero Episodi"}, {4, "Numero Stagioni"}, {5, "Durata Media Episodio"}},
                 {{3, "Numero Episodi"}, {4, "Numero Stagioni"}, {5, "Durata Media Episodio"}},
                 {{6, "In Corso"}}}},
    {"Anime", {MIN_SERIES_FIELDS,
              {{2, "Anno"}, {3, "Numero Episodi"}, {4, "Numero Stagioni"}, {5, "Durata Media Episodio"}},
              {{3, "Numero Episodi"}, {4, "Numero Stagioni"}, {5, "Durata Media Episodio"}},
              {{6, "In Corso"}}}},
    {"Libro", {MIN_BOOK_FIELDS,
              {{2, "Anno"}, {4, "Anno Stampa"}},
              {},
              {}}},
    {"Manga", {MIN_MANGA_FIELDS,
              {{2, "Anno"}, {5, "Numero Libri"}},
              {{5, "Numero Libri"}},
              {{6, "Concluso"}}}},
    {"Cd", {MIN_CD_FIELDS,
           {{2, "Anno"}, {4, "Numero Tracce"}, {5, "Durata Media Tracce"}},
           {{4, "Numero Tracce"}, {5, "Durata Media Tracce"}},
           {}}}
};

MediaService::MediaService(QObject *parent) : QObject(parent), jsonService(nullptr)
{
    initializeMediaFactories();
}

MediaService::~MediaService()
{
    clearMediaCollection();
}

void MediaService::initializeMediaFactories() {
    // Factory per la creazione dei media con parametri da form
    mediaCreationFactories["Serie Tv"] = [this](const QList<QLineEdit*>& fields) -> Media* {
        if (fields.size() < MIN_SERIES_FIELDS) return nullptr;
        
        bool inCorso = parseBoolFromString(fields[6]->text(), "In Corso");
        return new SerieTv(
            fields[0]->text().toStdString(),  // titolo
            fields[2]->text().toInt(),        // anno
            fields[1]->text().toStdString(),  // immagine
            fields[3]->text().toInt(),        // numEpisodi
            fields[4]->text().toInt(),        // numStagioni
            fields[5]->text().toInt(),        // durataMediaEp
            inCorso,                          // inCorso
            fields[7]->text().toStdString(),  // ideatore
            fields[8]->text().toStdString()   // casaProduttrice
        );
    };
    
    mediaCreationFactories["Anime"] = [this](const QList<QLineEdit*>& fields) -> Media* {
        if (fields.size() < MIN_SERIES_FIELDS) return nullptr;
        
        bool inCorso = parseBoolFromString(fields[6]->text(), "In Corso");
        return new Anime(
            fields[0]->text().toStdString(),  // titolo
            fields[2]->text().toInt(),        // anno
            fields[1]->text().toStdString(),  // immagine
            fields[3]->text().toInt(),        // numEpisodi
            fields[4]->text().toInt(),        // numStagioni
            fields[5]->text().toInt(),        // durataMediaEp
            inCorso,                          // inCorso
            fields[7]->text().toStdString(),  // disegnatore
            fields[8]->text().toStdString()   // studioAnimazione
        );
    };
    
    mediaCreationFactories["Film"] = [this](const QList<QLineEdit*>& fields) -> Media* {
        if (fields.size() < MIN_FILM_FIELDS) return nullptr;
        
        return new Film(
            fields[0]->text().toStdString(),  // titolo
            fields[2]->text().toInt(),        // anno
            fields[1]->text().toStdString(),  // immagine
            fields[3]->text().toStdString(),  // regista
            fields[4]->text().toStdString(),  // attoreProtagonista
            fields[5]->text().toInt()         // durata
        );
    };
    
    mediaCreationFactories["Libro"] = [this](const QList<QLineEdit*>& fields) -> Media* {
        if (fields.size() < MIN_BOOK_FIELDS) return nullptr;
        
        return new Libro(
            fields[0]->text().toStdString(),  // titolo
            fields[2]->text().toInt(),        // anno
            fields[1]->text().toStdString(),  // immagine
            fields[3]->text().toStdString(),  // scrittore
            fields[4]->text().toInt(),        // annoStampa
            fields[5]->text().toStdString()   // casaEditrice
        );
    };
    
    mediaCreationFactories["Manga"] = [this](const QList<QLineEdit*>& fields) -> Media* {
        if (fields.size() < MIN_MANGA_FIELDS) return nullptr;
        
        bool concluso = parseBoolFromString(fields[6]->text(), "Concluso");
        return new Manga(
            fields[0]->text().toStdString(),  // titolo
            fields[2]->text().toInt(),        // anno
            fields[1]->text().toStdString(),  // immagine
            fields[3]->text().toStdString(),  // scrittore
            fields[4]->text().toStdString(),  // illustratore
            fields[5]->text().toInt(),        // numLibri
            concluso                          // concluso
        );
    };
    
    mediaCreationFactories["Cd"] = [this](const QList<QLineEdit*>& fields) -> Media* {
        if (fields.size() < MIN_CD_FIELDS) return nullptr;
        
        return new Cd(
            fields[0]->text().toStdString(),  // titolo
            fields[2]->text().toInt(),        // anno
            fields[1]->text().toStdString(),  // immagine
            fields[3]->text().toStdString(),  // artista
            fields[4]->text().toInt(),        // numTracce
            fields[5]->text().toInt()         // durataMedTracce
        );
    };
}

void MediaService::setMediaCollection(const QVector<Media*>& collection)
{
    clearMediaCollection();
    mediaCollection = collection;
    updateJsonService();
    emit collectionChanged();
}

QVector<Media*> MediaService::getMediaCollection() const
{
    return mediaCollection;
}

void MediaService::clearMediaCollection()
{
    for (Media* media : mediaCollection) {
        delete media;
    }
    mediaCollection.clear();
    updateJsonService();
    emit collectionChanged();
}

bool MediaService::addMedia(Media* media)
{
    if (!media) return false;
    
    mediaCollection.append(media);
    updateJsonService();
    emit mediaAdded(media);
    emit collectionChanged();
    return true;
}

bool MediaService::updateMedia(Media* oldMedia, Media* newMedia)
{
    if (!oldMedia || !newMedia) return false;
    
    int index = mediaCollection.indexOf(oldMedia);
    if (index == -1) return false;
    
    mediaCollection[index] = newMedia;
    delete oldMedia;
    updateJsonService();
    emit mediaUpdated(oldMedia, newMedia);
    emit collectionChanged();
    return true;
}

bool MediaService::deleteMedia(Media* media)
{
    if (!media) return false;
    
    int index = mediaCollection.indexOf(media);
    if (index == -1) return false;
    
    mediaCollection.removeAt(index);
    delete media;
    updateJsonService();
    emit mediaDeleted(media);
    emit collectionChanged();
    return true;
}

Media* MediaService::findMediaByTitle(const QString& title) const
{
    for (Media* media : mediaCollection) {
        if (QString::fromStdString(media->getTitolo()) == title) {
            return media;
        }
    }
    return nullptr;
}

bool MediaService::loadFromFile(const QString& filePath)
{
    if (!jsonService) return false;
    
    currentFilePath = filePath;
    bool success = jsonService->loadFromFile(filePath);
    if (success) {
        setMediaCollection(jsonService->getAllMedia());
        emit dataLoaded();
    }
    return success;
}

bool MediaService::saveToFile(const QString& filePath)
{
    if (!jsonService) return false;
    
    currentFilePath = filePath;
    bool success = jsonService->saveToFile(filePath);
    if (success) {
        emit dataSaved();
    }
    return success;
}

void MediaService::setJsonService(JsonService* service)
{
    jsonService = service;
}

QVector<Media*> MediaService::filterMedia(const QString& category, const QString& searchText) const
{
    QVector<Media*> filtered;
    std::string categoryStd = category.toStdString();
    std::string searchStd = searchText.toStdString();
    
    for (Media* media : mediaCollection) {
        if (media->matchesCategory(categoryStd) && media->matchesSearch(searchStd)) {
            filtered.append(media);
        }
    }
    return filtered;
}

int MediaService::getMediaCount() const
{
    return mediaCollection.size();
}

bool MediaService::isEmpty() const
{
    return mediaCollection.isEmpty();
}

void MediaService::updateJsonService()
{
    if (jsonService) {
        jsonService->clearAll();
        for (Media* media : mediaCollection) {
            jsonService->addMedia(media);
        }
    }
}

bool MediaService::parseBoolFromString(const QString& text, const QString& fieldName) const
{
    QString lowered = text.trimmed().toLower();
    
    if (TRUE_VALUES.contains(lowered)) return true;
    if (FALSE_VALUES.contains(lowered)) return false;

    throw std::invalid_argument(
        QString("Valore non valido per '%1'. Usa: true/false, 1/0, si/no, yes/no")
            .arg(fieldName).toStdString()
    );
}

bool MediaService::validateRequiredFields(const QList<QLineEdit*>& fields, QWidget* parent) const
{
    for (QLineEdit* field : fields) {
        if (field->text().trimmed().isEmpty()) {
            QMessageBox::warning(parent,
                "Attenzione",
                QString("Il campo '%1' è obbligatorio!").arg(field->placeholderText()));
            field->setFocus();
            return false;
        }
    }
    return true;
}

bool MediaService::validateFieldsByType(const QString& type, const QList<QLineEdit*>& fields, QWidget* parent) const {
    auto config = validationConfigs.find(type);
    if (config == validationConfigs.end()) return true;
    
    if (fields.size() < config->minFields) return false;
    
    // Validazione unificata con lambda, non sicuro dell utilizzo corretto
    auto validateFields = [this, &fields, parent](const QVector<QPair<int, QString>>& fieldConfigs, 
                                                   auto validator) -> bool {
        for (const auto& field : fieldConfigs) {
            if (!validateFieldAtIndex(fields, field.first, field.second, parent, validator)) {
                return false;
            }
        }
        return true;
    };
    
    // Applica validazioni specifiche
    return validateFields(config->integerFields, 
                         [this](QLineEdit* f, const QString& n, QWidget* p) { 
                             return validateInteger(f, n, p); 
                         }) &&
           validateFields(config->positiveFields,
                         [this](QLineEdit* f, const QString& n, QWidget* p) { 
                             return validatePositiveInteger(f, n, p); 
                         }) &&
           validateFields(config->booleanFields,
                         [this](QLineEdit* f, const QString& n, QWidget* p) { 
                             return validateBoolean(f, n, p); 
                         });
}

bool MediaService::validateInteger(QLineEdit* field, const QString& fieldName, QWidget* parent) const {
    bool ok;
    field->text().trimmed().toInt(&ok);
    
    if (!ok) {
        QMessageBox::warning(parent, "Errore di Validazione",
            QString("Il campo '%1' deve essere un numero intero valido.\nValore inserito: '%2'")
                .arg(fieldName, field->text()));
        field->setFocus();
        field->selectAll();
        return false;
    }
    return true;
}

bool MediaService::validatePositiveInteger(QLineEdit* field, const QString& fieldName, QWidget* parent) const {
    bool ok;
    int value = field->text().trimmed().toInt(&ok);
    
    if (!ok || value <= 0) {
        QMessageBox::warning(parent, "Errore di Validazione",
            QString("Il campo '%1' deve essere un numero positivo (maggiore di 0).\nValore inserito: '%2'")
                .arg(fieldName, field->text()));
        field->setFocus();
        field->selectAll();
        return false;
    }
    return true;
}

bool MediaService::validateBoolean(QLineEdit* field, const QString& fieldName, QWidget* parent) const {
    QString text = field->text().trimmed().toLower();
    QStringList allValidValues = TRUE_VALUES + FALSE_VALUES;
    
    if (!allValidValues.contains(text)) {
        QMessageBox::warning(parent, "Errore di Validazione",
            QString("Il campo '%1' deve essere un valore booleano valido.\n\n"
                   "Valori accettati: %2\n"
                   "Valore inserito: '%3'")
                .arg(fieldName, allValidValues.join(", "), field->text()));
        field->setFocus();
        field->selectAll();
        return false;
    }
    return true;
}

Media* MediaService::createMediaFromFields(const QString& type, const QList<QLineEdit*>& fields, QWidget* parent)
{
    // Validazione campi obbligatori
    if (!validateRequiredFields(fields, parent)) {
        return nullptr;
    }

    try {
        // Validazione specifica per tipo
        if (!validateFieldsByType(type, fields, parent)) {
            return nullptr;
        }
        
        // Usa la factory per creare il media
        auto factoryIt = mediaCreationFactories.find(type.toStdString());
        if (factoryIt != mediaCreationFactories.end()) {
            return factoryIt->second(fields);
        }
        
        QMessageBox::critical(parent,
            "Errore",
            QString("Tipo di media non supportato: %1").arg(type));
        return nullptr;
    }
    catch (const std::exception& e) {
        QMessageBox::critical(parent,
            "Errore",
            QString("Dati non validi: %1").arg(e.what()));
        return nullptr;
    }
    catch (...) {
        QMessageBox::critical(parent,
            "Errore",
            "Si è verificato un errore sconosciuto durante la creazione del media");
        return nullptr;
    }
}