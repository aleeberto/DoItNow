#include "jsonService.h"
#include "../logic/appointment.h"
#include "../logic/deadline.h"
#include "../logic/recursive.h"
#include "../logic/reminder.h"

JsonService::JsonService(QObject *parent) : QObject(parent) {
    initializeFactories();
}

void JsonService::initializeFactories() {
    // Factory per Appointment
    eventFactories["Appointment"] = [](const QJsonObject& json) -> std::unique_ptr<Event> {
        auto appointment = std::make_unique<Appointment>("", "", "default", 0, 0, 0, 0);
        appointment->fromJsonSpecific(json);
        // Carica anche i campi base
        appointment->setName(json["name"].toString().toStdString());
        appointment->setNote(json["note"].toString().toStdString());
        appointment->setImage(json["image"].toString().toStdString());
        return appointment;
    };
    
    // Factory per Deadline
    eventFactories["Deadline"] = [](const QJsonObject& json) -> std::unique_ptr<Event> {
        auto deadline = std::make_unique<Deadline>("", "", "default", 0, false, 0);
        deadline->fromJsonSpecific(json);
        // Carica anche i campi base
        deadline->setName(json["name"].toString().toStdString());
        deadline->setNote(json["note"].toString().toStdString());
        deadline->setImage(json["image"].toString().toStdString());
        return deadline;
    };
    
    // Factory per Recursive
    eventFactories["Recursive"] = [](const QJsonObject& json) -> std::unique_ptr<Event> {
        auto recursive = std::make_unique<Recursive>("", "", "default", 0, "");
        recursive->fromJsonSpecific(json);
        // Carica anche i campi base
        recursive->setName(json["name"].toString().toStdString());
        recursive->setNote(json["note"].toString().toStdString());
        recursive->setImage(json["image"].toString().toStdString());
        return recursive;
    };
    
    // Factory per Reminder
    eventFactories["Reminder"] = [](const QJsonObject& json) -> std::unique_ptr<Event> {
        auto reminder = std::make_unique<Reminder>("", "", "default", "");
        reminder->fromJsonSpecific(json);
        // Carica anche i campi base
        reminder->setName(json["name"].toString().toStdString());
        reminder->setNote(json["note"].toString().toStdString());
        reminder->setImage(json["image"].toString().toStdString());
        return reminder;
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
    
    eventArray = doc.object()["events"].toArray();
    return true;
}

bool JsonService::saveToFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Impossibile salvare il file:" << filePath;
        return false;
    }
    
    QJsonObject rootObject;
    rootObject["events"] = eventArray;
    
    QJsonDocument doc(rootObject);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

QVector<Event*> JsonService::getAllEvents() const {
    QVector<Event*> result;
    
    for (const QJsonValue &value : eventArray) {
        auto event = createEventFromJson(value.toObject());
        if (event) {
            result.append(event.release());
        }
    }
    
    return result;
}

void JsonService::addEvent(Event *event) {
    if (!event) return;
    eventArray.append(eventToJson(event));
}

void JsonService::clearAll() {
    eventArray = QJsonArray();
}

std::unique_ptr<Event> JsonService::createEventFromJson(const QJsonObject &jsonObj) const {
    QString type = jsonObj["type"].toString();
    std::string typeStr = type.toStdString();
    
    auto factoryIt = eventFactories.find(typeStr);
    if (factoryIt != eventFactories.end()) {
        return factoryIt->second(jsonObj);
    }
    
    qWarning() << "Tipo di evento sconosciuto:" << type;
    return nullptr;
}

QJsonObject JsonService::eventToJson(Event *event) {
    if (!event) return QJsonObject();
    
    QJsonObject jsonObj;
    
    // Campi comuni
    jsonObj["name"] = QString::fromStdString(event->getName());
    jsonObj["note"] = QString::fromStdString(event->getNote());
    jsonObj["image"] = QString::fromStdString(event->getImage());
    
    // Campi specifici
    QJsonObject specificData = event->toJsonSpecific();
    for (auto it = specificData.begin(); it != specificData.end(); ++it) {
        jsonObj[it.key()] = it.value();
    }
    
    return jsonObj;
}