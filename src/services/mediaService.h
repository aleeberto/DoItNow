#ifndef MEDIASERVICE_HEADER
#define MEDIASERVICE_HEADER

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
#include "../logic/media.h"
#include "jsonService.h"

class MediaService : public QObject
{
    Q_OBJECT

public:
    explicit MediaService(QObject *parent = nullptr);
    ~MediaService();

    void setMediaCollection(const QVector<Media*>& collection);
    QVector<Media*> getMediaCollection() const;
    void clearMediaCollection();

    bool addMedia(Media* media);
    bool updateMedia(Media* oldMedia, Media* newMedia);
    bool deleteMedia(Media* media);
    Media* findMediaByTitle(const QString& title) const;

    bool loadFromFile(const QString& filePath);
    bool saveToFile(const QString& filePath);
    void setJsonService(JsonService* service);

    QVector<Media*> filterMedia(const QString& category, const QString& searchText) const;

    int getMediaCount() const;
    bool isEmpty() const;

    Media* createMediaFromFields(const QString& type, const QList<QLineEdit*>& fields, QWidget* parent);

signals:
    void mediaAdded(Media* media);
    void mediaUpdated(Media* oldMedia, Media* newMedia);
    void mediaDeleted(Media* media);
    void collectionChanged();
    void dataLoaded();
    void dataSaved();

private:
    // Costanti per validazione
    static const QStringList TRUE_VALUES;
    static const QStringList FALSE_VALUES;
    static constexpr int MIN_FILM_FIELDS = 6;
    static constexpr int MIN_SERIES_FIELDS = 9;
    static constexpr int MIN_BOOK_FIELDS = 6;
    static constexpr int MIN_MANGA_FIELDS = 7;
    static constexpr int MIN_CD_FIELDS = 6;

    struct ValidationConfig {
        int minFields;
        QVector<QPair<int, QString>> integerFields;
        QVector<QPair<int, QString>> positiveFields;
        QVector<QPair<int, QString>> booleanFields;
    };

    QVector<Media*> mediaCollection;
    JsonService* jsonService;
    QString currentFilePath;
    
    // Factory map per la creazione dei media da form
    std::unordered_map<std::string, std::function<Media*(const QList<QLineEdit*>&)>> mediaCreationFactories;
    
    static const QMap<QString, ValidationConfig> validationConfigs;

    void updateJsonService();
    void initializeMediaFactories();
    
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
bool MediaService::validateFieldAtIndex(const QList<QLineEdit*>& fields, int index, 
                                       const QString& fieldName, QWidget* parent, 
                                       Validator validator) const {
    if (index >= fields.size()) return false;
    return validator(fields[index], fieldName, parent);
}

#endif