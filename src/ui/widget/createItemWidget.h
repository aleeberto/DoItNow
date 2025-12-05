#ifndef CREATEITEMWIDGET_HEADER
#define CREATEITEMWIDGET_HEADER

#include <QWidget>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QResizeEvent>
#include <QMessageBox>
#include <QMap>
#include "../../logic/event.h"
#include "../../services/eventService.h"

// Forward declaration
class FormWidgetVisitor;

class CreateItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CreateItemWidget(QWidget *parent = nullptr);
    
    // Metodi principali
    Event* createEventItem() const;
    void setEventService(EventService* service);
    
    // Gestione modalità e stile
    void setEditMode(Event* event);
    void resetToCreateMode();
    bool isInEditMode() const;
    void refreshStyles();

signals:
    void itemCreated(Event* newItem);
    void itemUpdated(Event* oldEvent, Event* newEvent);

public slots:
    void onItemTypeChanged(int index);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onCreateButtonClicked();

private:
    // Setup methods
    void setupTypeSelection();
    void updateScrollBarVisibility();
    
    // Edit mode helpers
    void populateFields(Event* event, const QString& eventType);
    void populateCommonFields(const QList<QLineEdit*>& fields, Event* event);
    void populateSeriesFields(const QList<QLineEdit*>& fields, int date, int value1, int value2);
    void clearEditMode();
    void clearAllFields();

    // UI Components
    QComboBox *itemTypeCombo;
    QStackedWidget *stackedFields;
    QPushButton *createButton;
    QScrollArea *scrollArea;
        
    // Services
    EventService* eventService;
    FormWidgetVisitor* formVisitor;
    
    // Widget
    QMap<QString, QWidget*> typeWidgets;

    // Edit mode state
    Event* editingEvent;
    bool editMode;
};

#endif // CREATEITEMWIDGET_HEADER