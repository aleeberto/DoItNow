#ifndef RIGHTLAYOUTWIDGET_HEADER
#define RIGHTLAYOUTWIDGET_HEADER

#include <QWidget>
#include <QVBoxLayout>
#include <QVector>
#include "../../logic/event.h"
#include "../../services/jsonService.h"
#include "../../services/uiService.h"

// Forward declaration
class EventWidgetVisitor;

class RightLayoutWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RightLayoutWidget(QWidget *parent = nullptr);
    ~RightLayoutWidget();

    void setEventCollection(const QVector<Event*>& collection);
    void setJsonService(JsonService* service);
    void setUIService(UIService* service);
    void displayEventCollection();
    void clearLayout();
    void showCreateItemWidget(QWidget* createWidget);
    void refreshStyles();

signals:
    void eventEditRequested(Event* event);
    void eventDeleteRequested(Event* event);

private slots:
    void onEditButtonClicked();
    void onDeleteButtonClicked();

private:
    void addEventCardToLayout(Event* event);

    QVBoxLayout *mainLayout;
    QVector<Event*> eventCollection;
    JsonService *jsonService;
    UIService *uiService;
    EventWidgetVisitor *widgetVisitor;
};

#endif // RIGHTLAYOUTWIDGET_HEADER