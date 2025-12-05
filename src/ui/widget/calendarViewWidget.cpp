#include "calendarViewWidget.h"
#include "../../services/styleUtils.h"
#include "../../logic/appointment.h"
#include "../../logic/deadline.h"
#include "../../logic/recursive.h"
#include "../../logic/reminder.h"
#include <QPushButton>
#include <QTextCharFormat>

CalendarViewWidget::CalendarViewWidget(QWidget *parent) 
    : QWidget(parent), uiService(nullptr)
{
    setupUI();
}

void CalendarViewWidget::setupUI()
{
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(StyleUtils::Dimensions::WIDGET_MARGIN,
                                   StyleUtils::Dimensions::WIDGET_MARGIN,
                                   StyleUtils::Dimensions::WIDGET_MARGIN,
                                   StyleUtils::Dimensions::WIDGET_MARGIN);
    mainLayout->setSpacing(StyleUtils::Dimensions::SPACING_XLARGE);

    // Pannello sinistro: Calendario
    QWidget* calendarPanel = new QWidget();
    QVBoxLayout* calendarLayout = new QVBoxLayout(calendarPanel);
    calendarLayout->setContentsMargins(0, 0, 0, 0);
    
    calendar = new QCalendarWidget();
    calendar->setGridVisible(true);
    calendar->setFirstDayOfWeek(Qt::Monday);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendar->setMinimumSize(350, 300);
    
    // Stile del calendario
    QString calendarStyle = QString(
        "QCalendarWidget {"
        "  background-color: %1;"
        "  border: 1px solid %2;"
        "  border-radius: %3px;"
        "}"
        "QCalendarWidget QToolButton {"
        "  color: %4;"
        "  background-color: %5;"
        "  border: none;"
        "  border-radius: 4px;"
        "  padding: 5px;"
        "  margin: 2px;"
        "}"
        "QCalendarWidget QToolButton:hover {"
        "  background-color: %6;"
        "}"
        "QCalendarWidget QMenu {"
        "  background-color: %7;"
        "  color: %8;"
        "}"
        "QCalendarWidget QSpinBox {"
        "  background-color: %9;"
        "  color: %10;"
        "  border: 1px solid %11;"
        "  border-radius: 4px;"
        "}"
        "QCalendarWidget QTableView {"
        "  background-color: %12;"
        "  selection-background-color: %13;"
        "  selection-color: white;"
        "  alternate-background-color: %14;"
        "}"
        "QCalendarWidget QTableView QHeaderView::section {"
        "  background-color: %15;"
        "  color: %16;"
        "  padding: 5px;"
        "  border: none;"
        "}"
    ).arg(StyleUtils::Colors::BACKGROUND())
     .arg(StyleUtils::Colors::BORDER())
     .arg(StyleUtils::Dimensions::BORDER_RADIUS)
     .arg(StyleUtils::Colors::TEXT())
     .arg(StyleUtils::Colors::SURFACE())
     .arg(StyleUtils::Colors::PRIMARY())
     .arg(StyleUtils::Colors::SURFACE())
     .arg(StyleUtils::Colors::TEXT())
     .arg(StyleUtils::Colors::BACKGROUND())
     .arg(StyleUtils::Colors::TEXT())
     .arg(StyleUtils::Colors::BORDER())
     .arg(StyleUtils::Colors::BACKGROUND())
     .arg(StyleUtils::Colors::PRIMARY())
     .arg(StyleUtils::Colors::SURFACE())
     .arg(StyleUtils::Colors::SURFACE())
     .arg(StyleUtils::Colors::TEXT_SECONDARY());
    
    calendar->setStyleSheet(calendarStyle);
    
    connect(calendar, &QCalendarWidget::clicked, this, &CalendarViewWidget::onDateClicked);
    
    calendarLayout->addWidget(calendar);
    calendarLayout->addStretch();
    
    // Pannello destro: Lista eventi del giorno selezionato
    QWidget* eventPanel = new QWidget();
    QVBoxLayout* eventPanelLayout = new QVBoxLayout(eventPanel);
    eventPanelLayout->setContentsMargins(0, 0, 0, 0);
    eventPanelLayout->setSpacing(StyleUtils::Dimensions::SPACING_MEDIUM);
    
    selectedDateLabel = new QLabel("Seleziona una data");
    selectedDateLabel->setStyleSheet(StyleUtils::getTitleLabelStyle());
    
    eventScrollArea = new QScrollArea();
    eventScrollArea->setWidgetResizable(true);
    eventScrollArea->setFrameShape(QFrame::NoFrame);
    eventScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    eventScrollArea->setStyleSheet(StyleUtils::getScrollAreaStyle());
    
    eventListWidget = new QWidget();
    eventListLayout = new QVBoxLayout(eventListWidget);
    eventListLayout->setContentsMargins(0, 0, 0, 0);
    eventListLayout->setSpacing(StyleUtils::Dimensions::SPACING_MEDIUM);
    eventListLayout->addStretch();
    
    eventScrollArea->setWidget(eventListWidget);
    
    eventPanelLayout->addWidget(selectedDateLabel);
    eventPanelLayout->addWidget(eventScrollArea, 1);
    
    mainLayout->addWidget(calendarPanel, 0);
    mainLayout->addWidget(eventPanel, 1);
}

void CalendarViewWidget::setEventCollection(const QVector<Event*>& collection)
{
    eventCollection = collection;
    eventsByDate.clear();
    
    // Organizza gli eventi per data
    for (Event* event : eventCollection) {
        Datable* datable = dynamic_cast<Datable*>(event);
        if (datable) {
            int dateInt = datable->getDate();
            eventsByDate[dateInt].append(event);
        }
    }
    
    highlightDatesWithEvents();
    
    // Aggiorna la lista per la data corrente se selezionata
    updateEventList(calendar->selectedDate());
}

void CalendarViewWidget::setUIService(UIService* service)
{
    uiService = service;
}

void CalendarViewWidget::onDateClicked(const QDate& date)
{
    selectedDateLabel->setText("Eventi del " + date.toString("dd/MM/yyyy"));
    updateEventList(date);
    emit dateSelected(date);
}

void CalendarViewWidget::updateEventList(const QDate& date)
{
    // Pulisci la lista attuale
    QLayoutItem* item;
    while ((item = eventListLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    int dateInt = dateToInt(date);
    
    // Aggiungi gli eventi del giorno
    if (eventsByDate.contains(dateInt)) {
        for (Event* event : eventsByDate[dateInt]) {
            QWidget* card = createEventCard(event);
            eventListLayout->addWidget(card);
        }
    }
    
    // Controlla eventi ricorrenti
    for (Event* event : eventCollection) {
        Recursive* recursive = dynamic_cast<Recursive*>(event);
        if (recursive) {
            QDate startDate = intToDate(recursive->getDate());
            QString recurrence = QString::fromStdString(recursive->getRecurrence());
            
            bool matches = false;
            
            if (recurrence == "daily") {
                matches = (date >= startDate);
            } else if (recurrence == "weekly") {
                matches = (date >= startDate && date.dayOfWeek() == startDate.dayOfWeek());
            } else if (recurrence == "monthly") {
                matches = (date >= startDate && date.day() == startDate.day());
            } else if (recurrence == "yearly") {
                matches = (date >= startDate && date.day() == startDate.day() && date.month() == startDate.month());
            }
            
            // Evita duplicati se la data coincide con la data di inizio
            if (matches && dateInt != recursive->getDate()) {
                QWidget* card = createEventCard(event);
                eventListLayout->addWidget(card);
            }
        }
    }
    
    // Se non ci sono eventi
    if (eventListLayout->count() == 0) {
        QLabel* noEventsLabel = new QLabel("Nessun evento per questa data");
        noEventsLabel->setStyleSheet(StyleUtils::getBodyLabelStyle());
        noEventsLabel->setAlignment(Qt::AlignCenter);
        eventListLayout->addWidget(noEventsLabel);
    }
    
    eventListLayout->addStretch();
}

void CalendarViewWidget::highlightDatesWithEvents()
{
    // Reset formato per tutte le date
    QTextCharFormat defaultFormat;
    defaultFormat.setBackground(Qt::transparent);
    
    // Formato per date con eventi
    QTextCharFormat eventFormat;
    eventFormat.setBackground(QColor(StyleUtils::Colors::PRIMARY()));
    eventFormat.setForeground(Qt::white);
    
    // Applica formato alle date con eventi
    for (auto it = eventsByDate.begin(); it != eventsByDate.end(); ++it) {
        QDate date = intToDate(it.key());
        if (date.isValid()) {
            calendar->setDateTextFormat(date, eventFormat);
        }
    }
}

QWidget* CalendarViewWidget::createEventCard(Event* event)
{
    QWidget* card = new QWidget();
    card->setStyleSheet(StyleUtils::getMediaCardStyle());
    
    QHBoxLayout* cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(StyleUtils::Dimensions::MARGIN_LARGE,
                                   StyleUtils::Dimensions::MARGIN_MEDIUM,
                                   StyleUtils::Dimensions::MARGIN_LARGE,
                                   StyleUtils::Dimensions::MARGIN_MEDIUM);
    
    // Info evento
    QWidget* infoWidget = new QWidget();
    QVBoxLayout* infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(StyleUtils::Dimensions::SPACING_SMALL);
    
    QLabel* nameLabel = new QLabel("<b>" + QString::fromStdString(event->getName()) + "</b>");
    nameLabel->setStyleSheet(StyleUtils::getSubtitleLabelStyle());
    
    QString typeText;
    QString detailsText;
    
    if (Appointment* apt = dynamic_cast<Appointment*>(event)) {
        typeText = "Appuntamento";
        if (uiService) {
            detailsText = "Ore " + uiService->formatTime(apt->getHour(), apt->getMinute()) + 
                         " - Durata: " + uiService->formatDuration(apt->getDurate());
        }
    } else if (Deadline* dl = dynamic_cast<Deadline*>(event)) {
        typeText = "Scadenza";
        QString stars = QString("★").repeated(dl->getImportance()) + 
                       QString("☆").repeated(5 - dl->getImportance());
        detailsText = "Importanza: " + stars;
    } else if (Recursive* rec = dynamic_cast<Recursive*>(event)) {
        typeText = "Ricorrente";
        QString recurrence = QString::fromStdString(rec->getRecurrence());
        if (recurrence == "daily") detailsText = "Giornaliero";
        else if (recurrence == "weekly") detailsText = "Settimanale";
        else if (recurrence == "monthly") detailsText = "Mensile";
        else if (recurrence == "yearly") detailsText = "Annuale";
    } else if (dynamic_cast<Reminder*>(event)) {
        typeText = "Promemoria";
    }
    
    QLabel* typeLabel = new QLabel(typeText);
    typeLabel->setStyleSheet(StyleUtils::getBodyLabelStyle());
    
    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(typeLabel);
    
    if (!detailsText.isEmpty()) {
        QLabel* detailsLabel = new QLabel(detailsText);
        detailsLabel->setStyleSheet(StyleUtils::getBodyLabelStyle());
        infoLayout->addWidget(detailsLabel);
    }
    
    // Bottoni azione
    QWidget* buttonsWidget = new QWidget();
    QVBoxLayout* buttonsLayout = new QVBoxLayout(buttonsWidget);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(StyleUtils::Dimensions::SPACING_SMALL);
    
    QPushButton* editButton = new QPushButton();
    editButton->setIcon(QIcon("../resources/icon/edit.png"));
    editButton->setFixedSize(36, 36);
    editButton->setStyleSheet(StyleUtils::getItemButtonStyle());
    editButton->setToolTip("Modifica");
    editButton->setProperty("event", QVariant::fromValue(static_cast<void*>(event)));
    editButton->setObjectName("editButton");
    connect(editButton, &QPushButton::clicked, this, &CalendarViewWidget::onEditButtonClicked);
    
    QPushButton* deleteButton = new QPushButton();
    deleteButton->setIcon(QIcon("../resources/icon/delete.png"));
    deleteButton->setFixedSize(36, 36);
    deleteButton->setStyleSheet(StyleUtils::getItemButtonStyle());
    deleteButton->setToolTip("Elimina");
    deleteButton->setProperty("event", QVariant::fromValue(static_cast<void*>(event)));
    deleteButton->setObjectName("deleteButton");
    connect(deleteButton, &QPushButton::clicked, this, &CalendarViewWidget::onDeleteButtonClicked);
    
    buttonsLayout->addWidget(editButton);
    buttonsLayout->addWidget(deleteButton);
    buttonsLayout->addStretch();
    
    cardLayout->addWidget(infoWidget, 1);
    cardLayout->addWidget(buttonsWidget, 0);
    
    return card;
}

void CalendarViewWidget::onEditButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    QVariant eventVariant = button->property("event");
    if (eventVariant.isValid()) {
        Event* event = static_cast<Event*>(eventVariant.value<void*>());
        if (event) {
            emit eventEditRequested(event);
        }
    }
}

void CalendarViewWidget::onDeleteButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    QVariant eventVariant = button->property("event");
    if (eventVariant.isValid()) {
        Event* event = static_cast<Event*>(eventVariant.value<void*>());
        if (event) {
            emit eventDeleteRequested(event);
        }
    }
}

int CalendarViewWidget::dateToInt(const QDate& date) const
{
    return date.year() * 10000 + date.month() * 100 + date.day();
}

QDate CalendarViewWidget::intToDate(int dateInt) const
{
    int year = dateInt / 10000;
    int month = (dateInt % 10000) / 100;
    int day = dateInt % 100;
    return QDate(year, month, day);
}

void CalendarViewWidget::refreshStyles()
{
    // Aggiorna stile calendario
    QString calendarStyle = QString(
        "QCalendarWidget {"
        "  background-color: %1;"
        "  border: 1px solid %2;"
        "  border-radius: %3px;"
        "}"
        "QCalendarWidget QToolButton {"
        "  color: %4;"
        "  background-color: %5;"
        "  border: none;"
        "  border-radius: 4px;"
        "  padding: 5px;"
        "  margin: 2px;"
        "}"
        "QCalendarWidget QToolButton:hover {"
        "  background-color: %6;"
        "}"
        "QCalendarWidget QMenu {"
        "  background-color: %7;"
        "  color: %8;"
        "}"
        "QCalendarWidget QSpinBox {"
        "  background-color: %9;"
        "  color: %10;"
        "  border: 1px solid %11;"
        "  border-radius: 4px;"
        "}"
        "QCalendarWidget QTableView {"
        "  background-color: %12;"
        "  selection-background-color: %13;"
        "  selection-color: white;"
        "  alternate-background-color: %14;"
        "}"
        "QCalendarWidget QTableView QHeaderView::section {"
        "  background-color: %15;"
        "  color: %16;"
        "  padding: 5px;"
        "  border: none;"
        "}"
    ).arg(StyleUtils::Colors::BACKGROUND())
     .arg(StyleUtils::Colors::BORDER())
     .arg(StyleUtils::Dimensions::BORDER_RADIUS)
     .arg(StyleUtils::Colors::TEXT())
     .arg(StyleUtils::Colors::SURFACE())
     .arg(StyleUtils::Colors::PRIMARY())
     .arg(StyleUtils::Colors::SURFACE())
     .arg(StyleUtils::Colors::TEXT())
     .arg(StyleUtils::Colors::BACKGROUND())
     .arg(StyleUtils::Colors::TEXT())
     .arg(StyleUtils::Colors::BORDER())
     .arg(StyleUtils::Colors::BACKGROUND())
     .arg(StyleUtils::Colors::PRIMARY())
     .arg(StyleUtils::Colors::SURFACE())
     .arg(StyleUtils::Colors::SURFACE())
     .arg(StyleUtils::Colors::TEXT_SECONDARY());
    
    calendar->setStyleSheet(calendarStyle);
    selectedDateLabel->setStyleSheet(StyleUtils::getTitleLabelStyle());
    eventScrollArea->setStyleSheet(StyleUtils::getScrollAreaStyle());
    
    // Re-highlight dates
    highlightDatesWithEvents();
    
    // Aggiorna la lista eventi
    updateEventList(calendar->selectedDate());
}