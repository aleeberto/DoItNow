#ifndef FORMWIDGETVISITOR_HEADER
#define FORMWIDGETVISITOR_HEADER

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QFileDialog>
#include <QDir>
#include <QVector>
#include "../../logic/eventVisitor.h"

class FormWidgetVisitor : public EventVisitor {
public:
    enum class FieldType { TEXT, INTEGER, POSITIVE_INTEGER, BOOLEAN, DATE, TIME, IMAGE };
    
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
    
    // Configurazioni standard per ogni tipo di evento
    static const QVector<FieldConfig> APPOINTMENT_FIELDS;
    static const QVector<FieldConfig> DEADLINE_FIELDS;
    static const QVector<FieldConfig> RECURSIVE_FIELDS;
    static const QVector<FieldConfig> REMINDER_FIELDS;
    
public:
    explicit FormWidgetVisitor(QWidget* parent);
    
    void visit(Appointment* appointment) override;
    void visit(Deadline* deadline) override;
    void visit(Recursive* recursive) override;
    void visit(Reminder* reminder) override;
    
    QWidget* getResultWidget() const;
    
private:
    QWidget* createStandardForm(const QVector<FieldConfig>& fields, 
                               const QStringList& values = QStringList());
    QWidget* createFieldWidget(const FieldConfig& config, const QString& value = QString());
    QLineEdit* createStandardLineEdit(const QString& placeholder);
    QWidget* createImageFieldWidget(const QString& placeholder, const QString& value);
    QWidget* createDateFieldWidget(const QString& value);
    QWidget* createTimeFieldWidget(const QString& value);
    
    template<typename EventType>
    void visitGeneric(EventType* event, const QVector<FieldConfig>& fieldConfigs);
    
    QStringList extractValues(Appointment* appointment);
    QStringList extractValues(Deadline* deadline);
    QStringList extractValues(Recursive* recursive);
    QStringList extractValues(Reminder* reminder);
};

// Template
template<typename EventType>
void FormWidgetVisitor::visitGeneric(EventType* event, const QVector<FieldConfig>& fieldConfigs) {
    QStringList values;
    if (event) {
        values = extractValues(event);
    }
    resultWidget = createStandardForm(fieldConfigs, values);
}

#endif // FORMWIDGETVISITOR_HEADER