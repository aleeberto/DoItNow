#include "rightLayoutWidget.h"
#include "mediaWidgetVisitor.h"
#include "../../services/styleUtils.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>

RightLayoutWidget::RightLayoutWidget(QWidget *parent) : QWidget(parent), jsonService(nullptr), uiService(nullptr), widgetVisitor(nullptr)
{
    // Imposta lo stile iniziale
    setStyleSheet(StyleUtils::getContentAreaStyle());

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
}

RightLayoutWidget::~RightLayoutWidget()
{
    clearLayout();
    delete widgetVisitor;
}

void RightLayoutWidget::setMediaCollection(const QVector<Media*>& collection)
{
    mediaCollection = collection;
}

void RightLayoutWidget::setJsonService(JsonService* service)
{
    jsonService = service;
}

void RightLayoutWidget::setUIService(UIService* service)
{
    uiService = service;
    // Inizializza il visitor quando UIService è disponibile
    if (uiService && !widgetVisitor) {
        widgetVisitor = new MediaWidgetVisitor(this, uiService);
    }
}

void RightLayoutWidget::displayMediaCollection()
{
    clearLayout();

    for (Media* media : mediaCollection) {
        addMediaCardToLayout(media);
    }

    mainLayout->addStretch();
}

void RightLayoutWidget::clearLayout()
{
    QLayoutItem* item;
    while ((item = mainLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            QWidget* widget = item->widget();
            if (widget->objectName() != "CreateItemWidget") {
                widget->deleteLater();
            } else {
                widget->hide();
                widget->setParent(nullptr);
            }
        }
        delete item;
    }
}

void RightLayoutWidget::showCreateItemWidget(QWidget* createWidget)
{
    // Controllo widget esistente
    if (createWidget->parent() == this && mainLayout->indexOf(createWidget) != -1) {
        return;
    }
    
    clearLayout();

    // Controllo che il widget non abbia un parent
    if (createWidget->parent() && createWidget->parent() != this) {
        createWidget->setParent(nullptr);
    }
    
    createWidget->setParent(this);
    createWidget->show();
    createWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(createWidget, 1);
}

void RightLayoutWidget::addMediaCardToLayout(Media* media)
{
    if (!media || !uiService || !widgetVisitor) return;
    
    // Usa il visitor per creare il widget specifico per il tipo di media
    media->accept(widgetVisitor);
    QWidget* mediaCard = widgetVisitor->getResultWidget();
    
    if (!mediaCard) return;

    // Trova e connetti i pulsanti usando il pattern visitor
    QList<QPushButton*> buttons = mediaCard->findChildren<QPushButton*>();
    for (QPushButton* button : buttons) {
        if (button->objectName() == "editButton") {
            connect(button, &QPushButton::clicked, this, &RightLayoutWidget::onEditButtonClicked);
        } else if (button->objectName() == "deleteButton") {
            connect(button, &QPushButton::clicked, this, &RightLayoutWidget::onDeleteButtonClicked);
        }
    }

    mainLayout->addWidget(mediaCard);
}

void RightLayoutWidget::onEditButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    QVariant mediaVariant = button->property("media");
    if (mediaVariant.isValid()) {
        Media* media = static_cast<Media*>(mediaVariant.value<void*>());
        if (media) {
            emit mediaEditRequested(media);
        }
    }
}

void RightLayoutWidget::onDeleteButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    QVariant mediaVariant = button->property("media");
    if (mediaVariant.isValid()) {
        Media* media = static_cast<Media*>(mediaVariant.value<void*>());
        if (media) {
            emit mediaDeleteRequested(media);
        }
    }
}

void RightLayoutWidget::refreshStyles()
{
    // Metodo per aggiornare gli stili quando cambia il tema
    setStyleSheet(StyleUtils::getContentAreaStyle());
    update();
}