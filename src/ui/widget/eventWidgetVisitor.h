#ifndef EVENTWIDGETVISITOR_HEADER
#define EVENTWIDGETVISITOR_HEADER

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <functional>
#include "../../logic/eventVisitor.h"
#include "../../services/uiService.h"

class EventWidgetVisitor : public EventVisitor {
private:
    QWidget* parentWidget;
    UIService* uiService;
    QWidget* resultWidget;
    
public:
    explicit EventWidgetVisitor(QWidget* parent, UIService* ui);
    
    void visit(Appointment* appointment) override;
    void visit(Deadline* deadline) override;
    void visit(Recursive* recursive) override;
    void visit(Reminder* reminder) override;
    
    QWidget* getResultWidget() const;
    
private:
    // Factory methods
    QWidget* createBaseWidget(Event* event);
    QHBoxLayout* createMainLayout(QWidget* widget);
    QLabel* createImageLabel(Event* event, const QString& eventType);
    QWidget* createDetailsWidget();
    QWidget* createButtonsWidget(Event* event);
    
    // Helper methods
    void addCommonInfo(QVBoxLayout* layout, Event* event);
    QLabel* createTypeLabel(const QString& type);
    QLabel* createDetailLabel(const QString& text);
    void addActionButtons(QVBoxLayout* layout, Event* event);
    
    // Template per struttura comune
    template<typename EventType>
    void createEventWidget(EventType* event, const QString& typeName,
                          const std::function<void(QVBoxLayout*, EventType*)>& addSpecificDetails);
};

#endif // EVENTWIDGETVISITOR_HEADER