#include "eventWidgetVisitor.h"
#include "../../logic/appointment.h"
#include "../../logic/deadline.h"
#include "../../logic/recursive.h"
#include "../../logic/reminder.h"
#include "../../services/styleUtils.h"

EventWidgetVisitor::EventWidgetVisitor(QWidget* parent, UIService* ui) 
    : parentWidget(parent), uiService(ui), resultWidget(nullptr) {}

QWidget* EventWidgetVisitor::getResultWidget() const {
    return resultWidget;
}

QWidget* EventWidgetVisitor::createBaseWidget(Event* event) {
    Q_UNUSED(event)
    QWidget* widget = new QWidget(parentWidget);
    widget->setStyleSheet(StyleUtils::getMediaCardStyle());
    return widget;
}

QHBoxLayout* EventWidgetVisitor::createMainLayout(QWidget* widget) {
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(
        StyleUtils::Dimensions::MARGIN_LARGE, 
        StyleUtils::Dimensions::MARGIN_LARGE, 
        StyleUtils::Dimensions::MARGIN_LARGE, 
        StyleUtils::Dimensions::MARGIN_LARGE
    );
    return layout;
}

QLabel* EventWidgetVisitor::createImageLabel(Event* event, const QString& eventType) {
    QLabel* imgLabel = new QLabel();
    imgLabel->setPixmap(uiService->loadEventImage(
        event->getImage(), 
        eventType.toStdString()
    ));
    imgLabel->setAlignment(Qt::AlignTop);
    return imgLabel;
}

QWidget* EventWidgetVisitor::createDetailsWidget() {
    QWidget* detailsWidget = new QWidget();
    QVBoxLayout* detailsLayout = new QVBoxLayout(detailsWidget);
    detailsLayout->setAlignment(Qt::AlignTop);
    return detailsWidget;
}

QWidget* EventWidgetVisitor::createButtonsWidget(Event* event) {
    QWidget* buttonsWidget = new QWidget();
    QVBoxLayout* buttonsLayout = new QVBoxLayout(buttonsWidget);
    buttonsLayout->setAlignment(Qt::AlignTop);
    buttonsLayout->setSpacing(StyleUtils::Dimensions::MARGIN_LARGE);

    addActionButtons(buttonsLayout, event);
    buttonsLayout->addStretch();
    
    return buttonsWidget;
}

void EventWidgetVisitor::addCommonInfo(QVBoxLayout* layout, Event* event) {
    QLabel* nameLabel = new QLabel(uiService->formatEventName(event));
    nameLabel->setStyleSheet(StyleUtils::getTitleLabelStyle());
    
    QString note = uiService->formatEventNote(event);
    if (!note.isEmpty()) {
        QLabel* noteLabel = new QLabel("Note: " + note);
        noteLabel->setStyleSheet(StyleUtils::getBodyLabelStyle());
        layout->addWidget(nameLabel);
        layout->addWidget(noteLabel);
    } else {
        layout->addWidget(nameLabel);
    }
}

QLabel* EventWidgetVisitor::createTypeLabel(const QString& type) {
    QLabel* typeLabel = new QLabel("Tipo: " + type);
    typeLabel->setStyleSheet(StyleUtils::getSubtitleLabelStyle());
    return typeLabel;
}

QLabel* EventWidgetVisitor::createDetailLabel(const QString& text) {
    QLabel* label = new QLabel(text);
    label->setStyleSheet(StyleUtils::getBodyLabelStyle());
    return label;
}

void EventWidgetVisitor::addActionButtons(QVBoxLayout* layout, Event* event) {
    QPushButton* editButton = new QPushButton();
    editButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);
    editButton->setToolTip("Modifica questo elemento");
    editButton->setIcon(QIcon("../resources/icon/edit.png"));
    editButton->setStyleSheet(StyleUtils::getItemButtonStyle());

    QPushButton* deleteButton = new QPushButton();
    deleteButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);
    deleteButton->setToolTip("Elimina questo elemento");
    deleteButton->setIcon(QIcon("../resources/icon/delete.png"));
    deleteButton->setStyleSheet(StyleUtils::getItemButtonStyle());

    editButton->setProperty("event", QVariant::fromValue(static_cast<void*>(event)));
    deleteButton->setProperty("event", QVariant::fromValue(static_cast<void*>(event)));
    editButton->setObjectName("editButton");
    deleteButton->setObjectName("deleteButton");

    layout->addWidget(editButton);
    layout->addWidget(deleteButton);
}

template<typename EventType>
void EventWidgetVisitor::createEventWidget(EventType* event, const QString& typeName,
                                          const std::function<void(QVBoxLayout*, EventType*)>& addSpecificDetails) {
    resultWidget = createBaseWidget(event);
    QHBoxLayout* mainLayout = createMainLayout(resultWidget);

    QLabel* imgLabel = createImageLabel(event, typeName);
    QWidget* detailsWidget = createDetailsWidget();
    QVBoxLayout* detailsLayout = static_cast<QVBoxLayout*>(detailsWidget->layout());

    addCommonInfo(detailsLayout, event);
    detailsLayout->addWidget(createTypeLabel(typeName));
    
    addSpecificDetails(detailsLayout, event);
    detailsLayout->addStretch();

    QWidget* buttonsWidget = createButtonsWidget(event);

    mainLayout->addWidget(imgLabel);
    mainLayout->addWidget(detailsWidget, 1);
    mainLayout->addWidget(buttonsWidget);
}

void EventWidgetVisitor::visit(Appointment* appointment) {
    createEventWidget<Appointment>(appointment, "Appointment", [this](QVBoxLayout* layout, Appointment* a) {
        layout->addWidget(createDetailLabel("Data: " + uiService->formatDate(a->getDate())));
        layout->addWidget(createDetailLabel("Ora: " + uiService->formatTime(a->getHour())));
        layout->addWidget(createDetailLabel("Durata: " + uiService->formatDuration(a->getDurate())));
    });
}

void EventWidgetVisitor::visit(Deadline* deadline) {
    createEventWidget<Deadline>(deadline, "Deadline", [this](QVBoxLayout* layout, Deadline* d) {
        layout->addWidget(createDetailLabel("Data: " + uiService->formatDate(d->getDate())));
        
        // Mostra importanza con stelle
        QString stars = QString("★").repeated(d->getImportance()) + 
                       QString("☆").repeated(5 - d->getImportance());
        layout->addWidget(createDetailLabel("Importanza: " + stars));
        
        layout->addWidget(createDetailLabel("Posticipabile: " + QString(d->getPostponable() ? "Sì" : "No")));
    });
}

void EventWidgetVisitor::visit(Recursive* recursive) {
    createEventWidget<Recursive>(recursive, "Recursive", [this](QVBoxLayout* layout, Recursive* r) {
        layout->addWidget(createDetailLabel("Data inizio: " + uiService->formatDate(r->getDate())));
        
        // Formatta la ricorrenza in modo leggibile
        QString recurrence = QString::fromStdString(r->getRecurrence());
        QString recurrenceText;
        if (recurrence == "daily") recurrenceText = "Giornaliera";
        else if (recurrence == "weekly") recurrenceText = "Settimanale";
        else if (recurrence == "monthly") recurrenceText = "Mensile";
        else if (recurrence == "yearly") recurrenceText = "Annuale";
        else recurrenceText = recurrence;
        
        layout->addWidget(createDetailLabel("Ricorrenza: " + recurrenceText));
    });
}

void EventWidgetVisitor::visit(Reminder* reminder) {
    createEventWidget<Reminder>(reminder, "Reminder", [this](QVBoxLayout* layout, Reminder* r) {
        QString longNote = QString::fromStdString(r->getLongNote());
        
        // Tronca se troppo lungo
        if (longNote.length() > 100) {
            longNote = longNote.left(97) + "...";
        }
        
        if (!longNote.isEmpty()) {
            layout->addWidget(createDetailLabel("Dettagli: " + longNote));
        }
    });
}