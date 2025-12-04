#include "createItemWidget.h"
#include "formWidgetVisitor.h"
#include <QScrollArea>
#include <QTimer>
#include <QAbstractScrollArea>
#include "../../services/styleUtils.h"
#include "../../logic/film.h"
#include "../../logic/serieTv.h"
#include "../../logic/anime.h"
#include "../../logic/libro.h"
#include "../../logic/manga.h"
#include "../../logic/cd.h"

CreateItemWidget::CreateItemWidget(QWidget *parent) : QWidget(parent), editingMedia(nullptr), editMode(false), mediaService(nullptr), formVisitor(nullptr)
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

    // Crea i form per ogni tipo di media usando il visitor
    QStringList mediaTypes = {"Film", "Serie Tv", "Anime", "Libro", "Manga", "Cd"};
    
    for (const QString& type : mediaTypes) {
        QWidget* formPage = nullptr;
        
        if (type == "Film") {
            formVisitor->visit((Film*)nullptr);
            formPage = formVisitor->getResultWidget();
        } else if (type == "Serie Tv") {
            formVisitor->visit((SerieTv*)nullptr);
            formPage = formVisitor->getResultWidget();
        } else if (type == "Anime") {
            formVisitor->visit((Anime*)nullptr);
            formPage = formVisitor->getResultWidget();
        } else if (type == "Libro") {
            formVisitor->visit((Libro*)nullptr);
            formPage = formVisitor->getResultWidget();
        } else if (type == "Manga") {
            formVisitor->visit((Manga*)nullptr);
            formPage = formVisitor->getResultWidget();
        } else if (type == "Cd") {
            formVisitor->visit((Cd*)nullptr);
            formPage = formVisitor->getResultWidget();
        }
        
        if (formPage) {
            typeWidgets[type] = formPage;
            stackedFields->addWidget(formPage);
            itemTypeCombo->addItem(type);
        }
    }

    // Selezione tipo di media da inserire
    QLabel *label = new QLabel("Seleziona il tipo di oggetto:", formWidget);
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

    createButton = new QPushButton("Crea Media", buttonContainer);
    createButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT_LARGE);
    createButton->setStyleSheet(StyleUtils::getPrimaryButtonStyle());

    buttonLayout->addStretch();
    buttonLayout->addWidget(createButton);
    buttonLayout->addStretch();

    mainLayout->addWidget(scrollArea, 1); // stretch = 1 occupa tutto lo spazio
    mainLayout->addWidget(buttonContainer, 0); // stretch = 0 altezza fissa bottone

    setLayout(mainLayout);
    
    // policy di dimensionamento tutto lo spazio disponibile
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

Media* CreateItemWidget::createMediaItem() const
{
    QString type = itemTypeCombo->currentText();
    QWidget* currentPage = stackedFields->currentWidget();
    if (!currentPage) return nullptr;
    
    QList<QLineEdit*> fields = currentPage->findChildren<QLineEdit*>();

    if (!mediaService) {
        QMessageBox::critical(const_cast<QWidget*>(parentWidget()),
            "Errore",
            "MediaService non inizializzato");
        return nullptr;
    }

    return mediaService->createMediaFromFields(type, fields, const_cast<QWidget*>(static_cast<const QWidget*>(this)));
}

void CreateItemWidget::onCreateButtonClicked()
{
    if (!createButton->isEnabled()) return;
    
    createButton->setEnabled(false);
    
    auto enableButton = [this]() { 
        if (createButton) createButton->setEnabled(true); 
    };
    QTimer::singleShot(500, this, enableButton);
    
    Media* newItem = createMediaItem();
    if (!newItem) return;
    
    if (editMode && editingMedia) {
        emit itemUpdated(editingMedia, newItem);
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

void CreateItemWidget::setEditMode(Media* media)
{
    if (!media) return;
    
    editingMedia = media;
    editMode = true;
    
    createButton->setText("Aggiorna Media");
    createButton->setStyleSheet(StyleUtils::getWarningButtonStyle());
    
    // Determina il tipo usando dynamic_cast
    QString mediaType;
    if (dynamic_cast<Film*>(media)) mediaType = "Film";
    else if (dynamic_cast<SerieTv*>(media)) mediaType = "Serie Tv";
    else if (dynamic_cast<Anime*>(media)) mediaType = "Anime";
    else if (dynamic_cast<Libro*>(media)) mediaType = "Libro";
    else if (dynamic_cast<Manga*>(media)) mediaType = "Manga";
    else if (dynamic_cast<Cd*>(media)) mediaType = "Cd";
    else return; // Tipo non riconosciuto
    
    int typeIndex = itemTypeCombo->findText(mediaType);
    if (typeIndex != -1) {
        itemTypeCombo->setCurrentIndex(typeIndex);
        stackedFields->setCurrentIndex(typeIndex);
        populateFields(media, mediaType);
    }
}

void CreateItemWidget::populateCommonFields(const QList<QLineEdit*>& fields, Media* media) {
    if (fields.size() >= 3) {
        fields[0]->setText(QString::fromStdString(media->getTitolo()));
        fields[1]->setText(QString::fromStdString(media->getImmagine()));
        fields[2]->setText(QString::number(media->getAnno()));
    }
}

void CreateItemWidget::populateSeriesFields(const QList<QLineEdit*>& fields, int numEpisodi, 
                                           int numStagioni, int durataMedia, bool inCorso,
                                           const std::string& creator, const std::string& company) {
    if (fields.size() >= 9) {
        fields[3]->setText(QString::number(numEpisodi));
        fields[4]->setText(QString::number(numStagioni));
        fields[5]->setText(QString::number(durataMedia));
        fields[6]->setText(inCorso ? "true" : "false");
        fields[7]->setText(QString::fromStdString(creator));
        fields[8]->setText(QString::fromStdString(company));
    }
}

void CreateItemWidget::populateFields(Media* media, const QString& mediaType)
{
    QWidget* currentForm = stackedFields->currentWidget();
    if (!currentForm) return;
    
    QList<QLineEdit*> fields = currentForm->findChildren<QLineEdit*>();
    
    // Popola campi comuni
    populateCommonFields(fields, media);
    
    // Campi specifici per tipo
    if (mediaType == "Film") {
        if (auto* film = dynamic_cast<Film*>(media); film && fields.size() >= 6) {
            fields[3]->setText(QString::fromStdString(film->getRegista()));
            fields[4]->setText(QString::fromStdString(film->getAttoreProtagonista()));
            fields[5]->setText(QString::number(film->getDurata()));
        }
    } else if (mediaType == "Serie Tv") {
        if (auto* serie = dynamic_cast<SerieTv*>(media); serie) {
            populateSeriesFields(fields, serie->getNumEpisodi(), serie->getNumStagioni(), 
                               serie->getDurataMediaEp(), serie->getInCorso(),
                               serie->getIdeatore(), serie->getCasaProduttrice());
        }
    } else if (mediaType == "Anime") {
        if (auto* anime = dynamic_cast<Anime*>(media); anime) {
            populateSeriesFields(fields, anime->getNumEpisodi(), anime->getNumStagioni(),
                               anime->getDurataMediaEp(), anime->getInCorso(),
                               anime->getDisegnatore(), anime->getStudioAnimazione());
        }
    } else if (mediaType == "Libro") {
        if (auto* libro = dynamic_cast<Libro*>(media); libro && fields.size() >= 6) {
            fields[3]->setText(QString::fromStdString(libro->getScrittore()));
            fields[4]->setText(QString::number(libro->getAnnoStampa()));
            fields[5]->setText(QString::fromStdString(libro->getCasaEditrice()));
        }
    } else if (mediaType == "Manga") {
        if (auto* manga = dynamic_cast<Manga*>(media); manga && fields.size() >= 7) {
            fields[3]->setText(QString::fromStdString(manga->getScrittore()));
            fields[4]->setText(QString::fromStdString(manga->getIllustratore()));
            fields[5]->setText(QString::number(manga->getNumLibri()));
            fields[6]->setText(manga->getConcluso() ? "true" : "false");
        }
    } else if (mediaType == "Cd") {
        if (auto* cd = dynamic_cast<Cd*>(media); cd && fields.size() >= 6) {
            fields[3]->setText(QString::fromStdString(cd->getArtista()));
            fields[4]->setText(QString::number(cd->getNumTracce()));
            fields[5]->setText(QString::number(cd->getDurataMedTracce()));
        }
    }
}

void CreateItemWidget::clearEditMode()
{
    editingMedia = nullptr;
    editMode = false;
    
    createButton->setText("Crea Media");
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
    editingMedia = nullptr;
    editMode = false;
    
    createButton->setText("Crea Media");
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

void CreateItemWidget::setMediaService(MediaService* service)
{
    mediaService = service;
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
