#include "eventService.h"
#include <QMessageBox>
#include "../logic/appointment.h"
#include "../logic/deadline.h"
#include "../logic/recursive.h"
#include "../logic/reminder.h"

// Definizione costanti statiche
const QStringList EventService::TRUE_VALUES = {"true", "1", "si", "yes"};
const QStringList EventService::FALSE_VALUES = {"false", "0", "no"};

// Definizione configurazioni di validazione
const QMap<QString, EventService::ValidationConfig> EventService::validationConfigs = {
    {"Appointment", {MIN_APPOINTMENT_FIELDS, 
                    {{2, "Data"}, {3, "Ora"}, {4, "Minuti"}, {5, "Durata"}}, 
                    {{3, "Ora"}, {5, "Durata"}}, 
                    {}}},
    {"Deadline", {MIN_DEADLINE_FIELDS,
                 {{2, "Data"}, {4, "Importanza"}},
                 {{4, "Importanza"}},
                 {{3, "Posticipabile"}}}},
    {"Recursive", {MIN_RECURSIVE_FIELDS,
                  {{2, "Data"}},
                  {},
                  {}}},
    {"Reminder", {MIN_REMINDER_FIELDS,
                 {},
                 {},
                 {}}}
};

EventService::EventService(QObject *parent) : QObject(parent), jsonService(nullptr)
{
    initializeEventFactories();
}

EventService::~EventService()
{
    clearEventCollection();
}

void EventService::initializeEventFactories() {
    // Factory per Appointment
    // Ordine campi form: name(0), note(1), date(2), hour(3), minute(4), durate(5), image(6)
    // Ordine costruttore: name, note, image, date, hour, minute, durate
    eventCreationFactories["Appointment"] = [this](const QList<QLineEdit*>& fields) -> Event* {
        if (fields.size() < MIN_APPOINTMENT_FIELDS) return nullptr;
        
        QString imagePath = fields[6]->text().trimmed();
        if (imagePath.isEmpty()) imagePath = "default";
        
        return new Appointment(
            fields[0]->text().toStdString(),  // name
            fields[1]->text().toStdString(),  // note
            imagePath.toStdString(),          // image
            fields[2]->text().toInt(),        // date
            fields[3]->text().toInt(),        // hour
            fields[4]->text().toInt(),        // minute
            fields[5]->text().toInt()         // durate
        );
    };
    
    // Factory per Deadline
    // Ordine campi form: name(0), note(1), date(2), postponable(3), importance(4), image(5)
    // Ordine costruttore: name, note, image, date, postponable, importance
    eventCreationFactories["Deadline"] = [this](const QList<QLineEdit*>& fields) -> Event* {
        if (fields.size() < MIN_DEADLINE_FIELDS) return nullptr;
        
        bool postponable = parseBoolFromString(fields[3]->text(), "Posticipabile");
        QString imagePath = fields[5]->text().trimmed();
        if (imagePath.isEmpty()) imagePath = "default";
        
        return new Deadline(
            fields[0]->text().toStdString(),  // name
            fields[1]->text().toStdString(),  // note
            imagePath.toStdString(),          // image
            fields[2]->text().toInt(),        // date
            postponable,                      // postponable
            fields[4]->text().toInt()         // importance
        );
    };
    
    // Factory per Recursive
    // Ordine campi form: name(0), note(1), date(2), recurrence(3), image(4)
    // Ordine costruttore: name, note, image, date, recurrence
    eventCreationFactories["Recursive"] = [this](const QList<QLineEdit*>& fields) -> Event* {
        if (fields.size() < MIN_RECURSIVE_FIELDS) return nullptr;
        
        QString imagePath = fields[4]->text().trimmed();
        if (imagePath.isEmpty()) imagePath = "default";
        
        return new Recursive(
            fields[0]->text().toStdString(),  // name
            fields[1]->text().toStdString(),  // note
            imagePath.toStdString(),          // image
            fields[2]->text().toInt(),        // date
            fields[3]->text().toStdString()   // recurrence
        );
    };
    
    // Factory per Reminder
    // Ordine campi form: name(0), note(1), longnote(2), image(3)
    // Ordine costruttore: name, note, image, longnote
    eventCreationFactories["Reminder"] = [this](const QList<QLineEdit*>& fields) -> Event* {
        if (fields.size() < MIN_REMINDER_FIELDS) return nullptr;
        
        QString imagePath = fields[3]->text().trimmed();
        if (imagePath.isEmpty()) imagePath = "default";
        
        return new Reminder(
            fields[0]->text().toStdString(),  // name
            fields[1]->text().toStdString(),  // note
            imagePath.toStdString(),          // image
            fields[2]->text().toStdString()   // longnote
        );
    };
}

void EventService::setEventCollection(const QVector<Event*>& collection)
{
    clearEventCollection();
    eventCollection = collection;
    updateJsonService();
    emit collectionChanged();
}

QVector<Event*> EventService::getEventCollection() const
{
    return eventCollection;
}

void EventService::clearEventCollection()
{
    for (Event* event : eventCollection) {
        delete event;
    }
    eventCollection.clear();
    updateJsonService();
    emit collectionChanged();
}

bool EventService::addEvent(Event* event)
{
    if (!event) return false;
    
    eventCollection.append(event);
    updateJsonService();
    emit eventAdded(event);
    emit collectionChanged();
    return true;
}

bool EventService::updateEvent(Event* oldEvent, Event* newEvent)
{
    if (!oldEvent || !newEvent) return false;
    
    int index = eventCollection.indexOf(oldEvent);
    if (index == -1) return false;
    
    eventCollection[index] = newEvent;
    delete oldEvent;
    updateJsonService();
    emit eventUpdated(oldEvent, newEvent);
    emit collectionChanged();
    return true;
}

bool EventService::deleteEvent(Event* event)
{
    if (!event) return false;
    
    int index = eventCollection.indexOf(event);
    if (index == -1) return false;
    
    eventCollection.removeAt(index);
    delete event;
    updateJsonService();
    emit eventDeleted(event);
    emit collectionChanged();
    return true;
}

Event* EventService::findEventByName(const QString& name) const
{
    for (Event* event : eventCollection) {
        if (QString::fromStdString(event->getName()) == name) {
            return event;
        }
    }
    return nullptr;
}

bool EventService::loadFromFile(const QString& filePath)
{
    if (!jsonService) return false;
    
    currentFilePath = filePath;
    bool success = jsonService->loadFromFile(filePath);
    if (success) {
        setEventCollection(jsonService->getAllEvents());
        emit dataLoaded();
    }
    return success;
}

bool EventService::saveToFile(const QString& filePath)
{
    if (!jsonService) return false;
    
    currentFilePath = filePath;
    bool success = jsonService->saveToFile(filePath);
    if (success) {
        emit dataSaved();
    }
    return success;
}

void EventService::setJsonService(JsonService* service)
{
    jsonService = service;
}

QVector<Event*> EventService::filterEvents(const QString& category, const QString& searchText) const
{
    QVector<Event*> filtered;
    std::string categoryStd = category.toStdString();
    std::string searchStd = searchText.toStdString();
    
    for (Event* event : eventCollection) {
        if (event->matchesCategory(categoryStd) && event->matchesSearch(searchStd)) {
            filtered.append(event);
        }
    }
    return filtered;
}

int EventService::getEventCount() const
{
    return eventCollection.size();
}

bool EventService::isEmpty() const
{
    return eventCollection.isEmpty();
}

void EventService::updateJsonService()
{
    if (jsonService) {
        jsonService->clearAll();
        for (Event* event : eventCollection) {
            jsonService->addEvent(event);
        }
    }
}

bool EventService::parseBoolFromString(const QString& text, const QString& fieldName) const
{
    QString lowered = text.trimmed().toLower();
    
    if (TRUE_VALUES.contains(lowered)) return true;
    if (FALSE_VALUES.contains(lowered)) return false;

    throw std::invalid_argument(
        QString("Valore non valido per '%1'. Usa: true/false, 1/0, si/no, yes/no")
            .arg(fieldName).toStdString()
    );
}

bool EventService::validateRequiredFields(const QList<QLineEdit*>& fields, QWidget* parent) const
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

bool EventService::validateFieldsByType(const QString& type, const QList<QLineEdit*>& fields, QWidget* parent) const {
    auto config = validationConfigs.find(type);
    if (config == validationConfigs.end()) return true;
    
    if (fields.size() < config->minFields) return false;
    
    auto validateFields = [this, &fields, parent](const QVector<QPair<int, QString>>& fieldConfigs, 
                                                   auto validator) -> bool {
        for (const auto& field : fieldConfigs) {
            if (!validateFieldAtIndex(fields, field.first, field.second, parent, validator)) {
                return false;
            }
        }
        return true;
    };
    
    // Validazione speciale per i minuti negli Appointment
    if (type == "Appointment" && fields.size() >= 5) {
        if (!validateMinute(fields[4], "Minuti", parent)) {
            return false;
        }
    }
    
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

bool EventService::validateInteger(QLineEdit* field, const QString& fieldName, QWidget* parent) const {
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

bool EventService::validatePositiveInteger(QLineEdit* field, const QString& fieldName, QWidget* parent) const {
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

bool EventService::validateBoolean(QLineEdit* field, const QString& fieldName, QWidget* parent) const {
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

bool EventService::validateMinute(QLineEdit* field, const QString& fieldName, QWidget* parent) const {
    bool ok;
    int value = field->text().trimmed().toInt(&ok);
    
    if (!ok || value < 0 || value > 59) {
        QMessageBox::warning(parent, "Errore di Validazione",
            QString("Il campo '%1' deve essere un numero tra 0 e 59.\nValore inserito: '%2'")
                .arg(fieldName, field->text()));
        field->setFocus();
        field->selectAll();
        return false;
    }
    return true;
}

Event* EventService::createEventFromFields(const QString& type, const QList<QLineEdit*>& fields, QWidget* parent)
{
    if (!validateRequiredFields(fields, parent)) {
        return nullptr;
    }

    try {
        if (!validateFieldsByType(type, fields, parent)) {
            return nullptr;
        }
        
        auto factoryIt = eventCreationFactories.find(type.toStdString());
        if (factoryIt != eventCreationFactories.end()) {
            return factoryIt->second(fields);
        }
        
        QMessageBox::critical(parent,
            "Errore",
            QString("Tipo di evento non supportato: %1").arg(type));
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
            "Si è verificato un errore sconosciuto durante la creazione dell'evento");
        return nullptr;
    }
}