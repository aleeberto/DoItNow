#ifndef FORMWIDGETVISITOR_HEADER
#define FORMWIDGETVISITOR_HEADER

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QDir>
#include <QVector>
#include "../../logic/mediaVisitor.h"

class FormWidgetVisitor : public MediaVisitor {
public:
    enum class FieldType { TEXT, INTEGER, POSITIVE_INTEGER, BOOLEAN, IMAGE };
    
    struct FieldConfig {
        QString label;
        QString placeholder;
        FieldType type;
        
        FieldConfig(const QString& l, const QString& p, FieldType t = FieldType::TEXT)
            : label(l), placeholder(p), type(t) {}
    };

private:
    QWidget* parentWidget;
    QWidget* resultWidget;
    
    // Configurazioni standard per ogni tipo di media
    static const QVector<FieldConfig> FILM_FIELDS;
    static const QVector<FieldConfig> SERIES_FIELDS;
    static const QVector<FieldConfig> ANIME_FIELDS;
    static const QVector<FieldConfig> BOOK_FIELDS;
    static const QVector<FieldConfig> MANGA_FIELDS;
    static const QVector<FieldConfig> CD_FIELDS;
    
public:
    explicit FormWidgetVisitor(QWidget* parent);
    
    void visit(Film* film) override;
    void visit(SerieTv* serieTv) override;
    void visit(Anime* anime) override;
    void visit(Libro* libro) override;
    void visit(Manga* manga) override;
    void visit(Cd* cd) override;
    
    QWidget* getResultWidget() const;
    
private:
    QWidget* createStandardForm(const QVector<FieldConfig>& fields, 
                               const QStringList& values = QStringList());
    QWidget* createFieldWidget(const FieldConfig& config, const QString& value = QString());
    QLineEdit* createStandardLineEdit(const QString& placeholder);
    QWidget* createImageFieldWidget(const QString& placeholder, const QString& value);
    
    template<typename MediaType>
    void visitGeneric(MediaType* media, const QVector<FieldConfig>& fieldConfigs);
    
    QStringList extractValues(Film* film);
    QStringList extractValues(SerieTv* serieTv);
    QStringList extractValues(Anime* anime);
    QStringList extractValues(Libro* libro);
    QStringList extractValues(Manga* manga);
    QStringList extractValues(Cd* cd);
};

// Template
template<typename MediaType>
void FormWidgetVisitor::visitGeneric(MediaType* media, const QVector<FieldConfig>& fieldConfigs) {
    QStringList values;
    if (media) {
        values = extractValues(media);
    }
    resultWidget = createStandardForm(fieldConfigs, values);
}

#endif // FORMWIDGETVISITOR_HEADER