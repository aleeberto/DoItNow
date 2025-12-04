#ifndef EVENTSERVICE_HEADER
#define EVENTSERVICE_HEADER

#include <QObject>
#include <QVector>
#include <QString>
#include <QList>
#include <QLineEdit>
#include <QWidget>
#include <QMessageBox>
#include <QMap>
#include <functional>
#include <unordered_map>
#include "../logic/event.h"
#include "jsonService.h"

class EventService : public QObject
{
    Q_OBJECT

public:
    explicit EventService(QObject *parent = nullptr);
    ~EventService();

    void setEventCollection(const QVector<Event*>& collection);
    QVector<Event*> getEventCollection() const;
    void clearEventCollection();

    bool addEvent(Event* event);
    bool updateEvent(Event* oldEvent, Event* newEvent);
    bool deleteEvent(Event* event);
    Event* findEventByName(const QString& name) const;

    bool loadFromFile(const QString& filePath);
    bool saveToFile(const QString& filePath);
    void setJsonService(JsonService* service);

    QVector<Event*> filterEvents(const QString& category, const QString& searchText) const;

    int getEventCount() const;
    bool isEmpty() const;

    Event* createEventFromFields(const QString& type, const QList<QLineEdit*>& fields, QWidget* parent);

signals:
    void eventAdded(Event* event);
    void eventUpdated(Event* oldEvent, Event* newEvent);
    void eventDeleted(Event* event);
    void collectionChanged();
    void dataLoaded();
    void dataSaved();

private:
    // Costanti per validazione
    static const QStringList TRUE_VALUES;
    static const QStringList FALSE_VALUES;
    static constexpr int MIN_APPOINTMENT_FIELDS = 6;
    static constexpr int MIN_DEADLINE_FIELDS = 6;
    static constexpr int MIN_RECURSIVE_FIELDS = 5;
    static constexpr int MIN_REMINDER_FIELDS = 4;

    struct ValidationConfig {
        int minFields;
        QVector<QPair<int, QString>> integerFields;
        QVector<QPair<int, QString>> positiveFields;
        QVector<QPair<int, QString>> booleanFields;
    };

    QVector<Event*> eventCollection;
    JsonService* jsonService;
    QString currentFilePath;
    
    // Factory map per la creazione degli eventi da form
    std::unordered_map<std::string, std::function<Event*(const QList<QLineEdit*>&)>> eventCreationFactories;
    
    static const QMap<QString, ValidationConfig> validationConfigs;

    void updateJsonService();
    void initializeEventFactories();
    
    // Metodi di validazione unificati
    bool validateRequiredFields(const QList<QLineEdit*>& fields, QWidget* parent) const;
    bool validateFieldsByType(const QString& type, const QList<QLineEdit*>& fields, QWidget* parent) const;
    bool parseBoolFromString(const QString& text, const QString& fieldName) const;
    
    // Validatori specifici
    bool validateInteger(QLineEdit* field, const QString& fieldName, QWidget* parent) const;
    bool validatePositiveInteger(QLineEdit* field, const QString& fieldName, QWidget* parent) const;
    bool validateBoolean(QLineEdit* field, const QString& fieldName, QWidget* parent) const;
    
    // Template per validazione
    template<typename Validator>
    bool validateFieldAtIndex(const QList<QLineEdit*>& fields, int index, 
                             const QString& fieldName, QWidget* parent, 
                             Validator validator) const;
};

// Template
template<typename Validator>
bool EventService::validateFieldAtIndex(const QList<QLineEdit*>& fields, int index, 
                                       const QString& fieldName, QWidget* parent, 
                                       Validator validator) const {
    if (index >= fields.size()) return false;
    return validator(fields[index], fieldName, parent);
}

#endif