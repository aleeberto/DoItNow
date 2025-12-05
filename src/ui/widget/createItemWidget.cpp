#include "createItemWidget.h"
#include "formWidgetVisitor.h"
#include <QScrollArea>
#include <QTimer>
#include <QAbstractScrollArea>
#include "../../services/styleUtils.h"
#include "../../logic/appointment.h"
#include "../../logic/deadline.h"
#include "../../logic/recursive.h"
#include "../../logic/reminder.h"

CreateItemWidget::CreateItemWidget(QWidget *parent) : QWidget(parent), editingEvent(nullptr), editMode(false), eventService(nullptr), formVisitor(nullptr)
{
    // Inizializza il visitor
    formVisitor = new FormWidgetVisitor(this);
    
    // No margin solo right layout + scroll area
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget *formWidget = new QWidget();
    QVBoxLayout *formLayout = new QVBoxLayout(formWidget);
    formLayout->setContentsMargins(StyleUtils::Dimensions::WIDGET_MARGIN, 
                                  StyleUtils::Dimensions::WIDGET_MARGIN, 
                                  StyleUtils::Dimensions::WIDGET_MARGIN, 
                                  StyleUtils::Dimensions::WIDGET_MARGIN);
    formLayout->setSpacing(StyleUtils::Dimensions::MARGIN_LARGE);

    setupTypeSelection();

    stackedFields = new QStackedWidget(formWidget);
    stackedFields->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    // Crea i form per ogni tipo di evento usando il visitor
    QStringList eventTypes = {"Appointment", "Deadline", "Recursive", "Reminder"};
    
    for (const QString& type : eventTypes) {
        QWidget* formPage = nullptr;
        
        if (type == "Appointment") {
            formVisitor->visit((Appointment*)nullptr);
            formPage = formVisitor->getResultWidget();
        } else if (type == "Deadline") {
            formVisitor->visit((Deadline*)nullptr);
            formPage = formVisitor->getResultWidget();
        } else if (type == "Recursive") {
            formVisitor->visit((Recursive*)nullptr);
            formPage = formVisitor->getResultWidget();
        } else if (type == "Reminder") {
            formVisitor->visit((Reminder*)nullptr);
            formPage = formVisitor->getResultWidget();
        }
        
        if (formPage) {
            typeWidgets[type] = formPage;
            stackedFields->addWidget(formPage);
            itemTypeCombo->addItem(type);
        }
    }

    // Selezione tipo di evento da inserire
    QLabel *label = new QLabel("Seleziona il tipo di evento:", formWidget);
    label->setStyleSheet(StyleUtils::getSubtitleLabelStyle());
    
    formLayout->addWidget(label);
    formLayout->addWidget(itemTypeCombo);
    formLayout->addWidget(stackedFields);

    // Creazione della scroll area del form
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(formWidget);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    // Bottone creazione Item (fixed in place)
    QWidget *buttonContainer = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setContentsMargins(StyleUtils::Dimensions::WIDGET_MARGIN, 
                                    StyleUtils::Dimensions::WIDGET_MARGIN, 
                                    StyleUtils::Dimensions::WIDGET_MARGIN, 
                                    StyleUtils::Dimensions::WIDGET_MARGIN);
    buttonLayout->setSpacing(0);

    createButton = new QPushButton("Crea Evento", buttonContainer);
    createButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT_LARGE);
    createButton->setStyleSheet(StyleUtils::getPrimaryButtonStyle());

    buttonLayout->addStretch();
    buttonLayout->addWidget(createButton);
    buttonLayout->addStretch();

    mainLayout->addWidget(scrollArea, 1);
    mainLayout->addWidget(buttonContainer, 0);

    setLayout(mainLayout);
    
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Connessioni
    connect(itemTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CreateItemWidget::onItemTypeChanged);
    connect(createButton, &QPushButton::clicked,
            this, &CreateItemWidget::onCreateButtonClicked);

    onItemTypeChanged(0);
    
    QTimer::singleShot(0, this, &CreateItemWidget::updateScrollBarVisibility);
}

void CreateItemWidget::setupTypeSelection() {
    itemTypeCombo = new QComboBox();
    itemTypeCombo->setFixedHeight(35);
    itemTypeCombo->setStyleSheet(StyleUtils::getComboBoxStyle());
}

void CreateItemWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateScrollBarVisibility();
}

void CreateItemWidget::updateScrollBarVisibility()
{
    if (!scrollArea || !scrollArea->widget()) {
        return;
    }
    
    int contentHeight = scrollArea->widget()->sizeHint().height();
    int viewportHeight = scrollArea->viewport()->height();
    
    if (contentHeight <= viewportHeight) {
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
}

Event* CreateItemWidget::createEventItem() const
{
    QString type = itemTypeCombo->currentText();
    QWidget* currentPage = stackedFields->currentWidget();
    if (!currentPage) return nullptr;
    
    QList<QLineEdit*> fields = currentPage->findChildren<QLineEdit*>();

    if (!eventService) {
        QMessageBox::critical(const_cast<QWidget*>(parentWidget()),
            "Errore",
            "EventService non inizializzato");
        return nullptr;
    }

    return eventService->createEventFromFields(type, fields, const_cast<QWidget*>(static_cast<const QWidget*>(this)));
}

void CreateItemWidget::onCreateButtonClicked()
{
    if (!createButton->isEnabled()) return;
    
    createButton->setEnabled(false);
    
    auto enableButton = [this]() { 
        if (createButton) createButton->setEnabled(true); 
    };
    QTimer::singleShot(500, this, enableButton);
    
    Event* newItem = createEventItem();
    if (!newItem) return;
    
    if (editMode && editingEvent) {
        emit itemUpdated(editingEvent, newItem);
        clearEditMode();
    } else {
        emit itemCreated(newItem);
        clearAllFields();
    }
}

void CreateItemWidget::onItemTypeChanged(int index)
{
    stackedFields->setCurrentIndex(index);
    QTimer::singleShot(0, this, &CreateItemWidget::updateScrollBarVisibility);
}

void CreateItemWidget::setEditMode(Event* event)
{
    if (!event) return;
    
    editingEvent = event;
    editMode = true;
    
    createButton->setText("Aggiorna Evento");
    createButton->setStyleSheet(StyleUtils::getWarningButtonStyle());
    
    // Determina il tipo usando dynamic_cast
    QString eventType;
    if (dynamic_cast<Appointment*>(event)) eventType = "Appointment";
    else if (dynamic_cast<Deadline*>(event)) eventType = "Deadline";
    else if (dynamic_cast<Recursive*>(event)) eventType = "Recursive";
    else if (dynamic_cast<Reminder*>(event)) eventType = "Reminder";
    else return; // Tipo non riconosciuto
    
    int typeIndex = itemTypeCombo->findText(eventType);
    if (typeIndex != -1) {
        itemTypeCombo->setCurrentIndex(typeIndex);
        stackedFields->setCurrentIndex(typeIndex);
        populateFields(event, eventType);
    }
}

void CreateItemWidget::populateCommonFields(const QList<QLineEdit*>& fields, Event* event) {
    if (fields.size() >= 2) {
        fields[0]->setText(QString::fromStdString(event->getName()));
        fields[1]->setText(QString::fromStdString(event->getNote()));
    }
}

void CreateItemWidget::populateSeriesFields(const QList<QLineEdit*>& fields, int date, int value1, int value2) {
    if (fields.size() >= 5) {
        fields[2]->setText(QString::number(date));
        fields[3]->setText(QString::number(value1));
        fields[4]->setText(QString::number(value2));
    }
}

void CreateItemWidget::populateFields(Event* event, const QString& eventType)
{
    QWidget* currentForm = stackedFields->currentWidget();
    if (!currentForm) return;
    
    QList<QLineEdit*> fields = currentForm->findChildren<QLineEdit*>();
    
    // Popola campi comuni
    populateCommonFields(fields, event);
    
    // Campi specifici per tipo
    if (eventType == "Appointment") {
        if (auto* appointment = dynamic_cast<Appointment*>(event); appointment && fields.size() >= 6) {
            fields[2]->setText(QString::number(appointment->getDate()));
            fields[3]->setText(QString::number(appointment->getHour()));
            fields[4]->setText(QString::number(appointment->getDurate()));
            fields[5]->setText(QString::fromStdString(appointment->getImage()));
        }
    } else if (eventType == "Deadline") {
        if (auto* deadline = dynamic_cast<Deadline*>(event); deadline && fields.size() >= 6) {
            fields[2]->setText(QString::number(deadline->getDate()));
            fields[3]->setText(deadline->getPostponable() ? "true" : "false");
            fields[4]->setText(QString::number(deadline->getImportance()));
            fields[5]->setText(QString::fromStdString(deadline->getImage()));
        }
    } else if (eventType == "Recursive") {
        if (auto* recursive = dynamic_cast<Recursive*>(event); recursive && fields.size() >= 5) {
            fields[2]->setText(QString::number(recursive->getDate()));
            fields[3]->setText(QString::fromStdString(recursive->getRecurrence()));
            fields[4]->setText(QString::fromStdString(recursive->getImage()));
        }
    } else if (eventType == "Reminder") {
        if (auto* reminder = dynamic_cast<Reminder*>(event); reminder && fields.size() >= 4) {
            fields[2]->setText(QString::fromStdString(reminder->getLongNote()));
            fields[3]->setText(QString::fromStdString(reminder->getImage()));
        }
    }
}

void CreateItemWidget::clearEditMode()
{
    editingEvent = nullptr;
    editMode = false;
    
    createButton->setText("Crea Evento");
    createButton->setStyleSheet(StyleUtils::getPrimaryButtonStyle());
    
    clearAllFields();
}

void CreateItemWidget::clearAllFields()
{
    for (auto it = typeWidgets.begin(); it != typeWidgets.end(); ++it) {
        QWidget* formWidget = it.value();
        if (formWidget) {
            QList<QLineEdit*> fields = formWidget->findChildren<QLineEdit*>();
            for (QLineEdit* field : fields) {
                if (field) {
                    field->clear();
                }
            }
        }
    }
}

void CreateItemWidget::resetToCreateMode()
{
    editingEvent = nullptr;
    editMode = false;
    
    createButton->setText("Crea Evento");
    createButton->setStyleSheet(StyleUtils::getPrimaryButtonStyle());
    
    clearAllFields();
    
    if (itemTypeCombo->count() > 0) {
        itemTypeCombo->setCurrentIndex(0);
        stackedFields->setCurrentIndex(0);
    }
}

bool CreateItemWidget::isInEditMode() const
{
    return editMode;
}

void CreateItemWidget::setEventService(EventService* service)
{
    eventService = service;
}

void CreateItemWidget::refreshStyles()
{
    // Aggiorna lo stile del widget principale
    setStyleSheet("");
    setStyleSheet(StyleUtils::getContentAreaStyle());
    
    // Aggiorna il ComboBox
    if (itemTypeCombo) {
        itemTypeCombo->setStyleSheet(StyleUtils::getComboBoxStyle());
    }
    
    // Aggiorna il bottone principale
    if (createButton) {
        if (editMode) {
            createButton->setStyleSheet(StyleUtils::getWarningButtonStyle());
        } else {
            createButton->setStyleSheet(StyleUtils::getPrimaryButtonStyle());
        }
    }
    
    // Aggiorna tutti i widget del form corrente
    for (auto it = typeWidgets.begin(); it != typeWidgets.end(); ++it) {
        QWidget* formWidget = it.value();
        if (formWidget) {
            // Aggiorna tutti i QLineEdit
            QList<QLineEdit*> lineEdits = formWidget->findChildren<QLineEdit*>();
            for (QLineEdit* lineEdit : lineEdits) {
                lineEdit->setStyleSheet(StyleUtils::getLineEditStyle());
            }
            
            // Aggiorna tutti i QPushButton (bottoni per selezione immagine)
            QList<QPushButton*> buttons = formWidget->findChildren<QPushButton*>();
            for (QPushButton* button : buttons) {
                button->setStyleSheet(StyleUtils::getItemButtonStyle());
            }
            
            // Aggiorna tutti i QLabel
            QList<QLabel*> labels = formWidget->findChildren<QLabel*>();
            for (QLabel* label : labels) {
                label->setStyleSheet(StyleUtils::getSubtitleLabelStyle());
            }
        }
    }
    
    update();
}