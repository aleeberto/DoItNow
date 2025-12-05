#ifndef CALENDARVIEWWIDGET_HEADER
#define CALENDARVIEWWIDGET_HEADER

#include <QWidget>
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDate>
#include <QVector>
#include <QMap>
#include "../../logic/event.h"
#include "../../logic/datable.h"
#include "../../services/uiService.h"

class CalendarViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarViewWidget(QWidget *parent = nullptr);
    ~CalendarViewWidget() = default;

    void setEventCollection(const QVector<Event*>& collection);
    void setUIService(UIService* service);
    void refreshStyles();

signals:
    void eventEditRequested(Event* event);
    void eventDeleteRequested(Event* event);
    void dateSelected(const QDate& date);

private slots:
    void onDateClicked(const QDate& date);
    void onEditButtonClicked();
    void onDeleteButtonClicked();

private:
    void setupUI();
    void updateEventList(const QDate& date);
    void highlightDatesWithEvents();
    QWidget* createEventCard(Event* event);
    int dateToInt(const QDate& date) const;
    QDate intToDate(int dateInt) const;

    QCalendarWidget* calendar;
    QWidget* eventListWidget;
    QVBoxLayout* eventListLayout;
    QScrollArea* eventScrollArea;
    QLabel* selectedDateLabel;
    UIService* uiService;
    
    QVector<Event*> eventCollection;
    QMap<int, QVector<Event*>> eventsByDate;  // Map date -> events
};

#endif // CALENDARVIEWWIDGET_HEADER