#include "mainWindow.h"
#include "widget/createItemWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QScrollArea>
#include <QLabel>
#include <QDir>
#include <QFile>
#include "../services/styleUtils.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->setStyleSheet(StyleUtils::getMainWindowStyle());
    setWindowTitle("Bibliotheca Procurator");

    setupUI();
    initializeServices();
    setupConnections();
    loadDefaultData();
}

MainWindow::~MainWindow()
{
    delete mediaService;
    delete jsonService;
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setStyleSheet(StyleUtils::getContentAreaStyle());
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupLeftPanel();
    setupRightPanel();

    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(rightScrollArea, 1);

    setMinimumSize(1150, 600);
}

void MainWindow::setupLeftPanel()
{
    leftWidget = new QWidget();
    leftWidget->setStyleSheet(StyleUtils::getSidebarStyle());
    leftWidget->setFixedWidth(StyleUtils::Dimensions::SIDEBAR_WIDTH);

    leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(StyleUtils::Dimensions::LAYOUT_MARGIN, 
                                  StyleUtils::Dimensions::MARGIN_LARGE, 
                                  StyleUtils::Dimensions::LAYOUT_MARGIN, 
                                  StyleUtils::Dimensions::MARGIN_LARGE);
    leftLayout->setSpacing(StyleUtils::Dimensions::LAYOUT_MARGIN);

    topMenu = new TopMenuWidget(this);
    leftLayout->addWidget(topMenu);

    setupSearchBar();
    setupCategoryButtons();
    leftLayout->addStretch();
}

void MainWindow::setupRightPanel()
{
    rightLayoutWidget = new RightLayoutWidget(this);
    
    rightScrollArea = new QScrollArea(this);
    rightScrollArea->setWidgetResizable(true);
    rightScrollArea->setWidget(rightLayoutWidget);
    rightScrollArea->setFrameShape(QFrame::NoFrame);
    rightScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MainWindow::initializeServices()
{
    jsonService = new JsonService(this);
    mediaService = new MediaService(this);
    mediaService->setJsonService(jsonService);
    
    uiService = new UIService(this);
    rightLayoutWidget->setUIService(uiService);

    createItemWidget = new CreateItemWidget(this);
    createItemWidget->setObjectName("CreateItemWidget");
    createItemWidget->setMediaService(mediaService);
    createItemWidget->hide();
}

void MainWindow::setupConnections()
{
    connect(topMenu, &TopMenuWidget::uploadRequested, this, &MainWindow::handleUploadRequest);
    connect(topMenu, &TopMenuWidget::createRequested, this, &MainWindow::showCreateItemWidget);
    connect(topMenu, &TopMenuWidget::exportRequested, this, &MainWindow::handleExportRequest);
    connect(topMenu, &TopMenuWidget::closeRequested, this, &MainWindow::handleCloseRequest);
    connect(topMenu, &TopMenuWidget::themeToggleRequested, this, &MainWindow::handleThemeToggle);
    connect(createItemWidget, &CreateItemWidget::itemCreated, this, &MainWindow::onMediaItemCreated);
    connect(createItemWidget, &CreateItemWidget::itemUpdated, this, &MainWindow::onMediaItemUpdated);
    connect(rightLayoutWidget, &RightLayoutWidget::mediaEditRequested, this, &MainWindow::onMediaEditRequested);
    connect(rightLayoutWidget, &RightLayoutWidget::mediaDeleteRequested, this, &MainWindow::onMediaDeleteRequested);
}

void MainWindow::loadDefaultData()
{
    if (QFile::exists(currentJsonPath)) {
        loadMediaData(currentJsonPath);
    }
}

void MainWindow::setupCategoryButtons()
{
    static const QStringList categories = {"Tutti", "Film", "Serie Tv", "Anime", "Libro", "Manga", "Cd"};

    for (const QString &category : categories) {
        QPushButton *btn = createCategoryButton(category);
        categoryButtons[category] = btn;
        leftLayout->addWidget(btn);
    }
}

QPushButton* MainWindow::createCategoryButton(const QString& category)
{
    QPushButton *btn = new QPushButton(category);
    btn->setCheckable(true);
    btn->setChecked(category == "Tutti");
    btn->setStyleSheet(StyleUtils::getCategoryButtonStyle());

    connect(btn, &QPushButton::clicked, this, [this, category]() {
        handleCategorySelection(category);
    });

    return btn;
}

void MainWindow::handleCategorySelection(const QString& category)
{
    currentCategory = category;
    
    QVector<Media*> filteredMedia = mediaService->filterMedia(category, searchBar->text());
    rightLayoutWidget->setMediaCollection(filteredMedia);
    rightLayoutWidget->setJsonService(jsonService);
    rightLayoutWidget->displayMediaCollection();

    updateCategoryButtonStates(category);
}

void MainWindow::updateCategoryButtonStates(const QString& selectedCategory)
{
    for (auto it = categoryButtons.begin(); it != categoryButtons.end(); ++it) {
        it.value()->setChecked(it.key() == selectedCategory);
    }
}

void MainWindow::setupSearchBar()
{
    searchBar = new QLineEdit();
    searchBar->setPlaceholderText("Cerca per titolo...");
    searchBar->setClearButtonEnabled(true);
    searchBar->setStyleSheet(StyleUtils::getSearchBarStyle());
    searchBar->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);

    connect(searchBar, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    leftLayout->addWidget(searchBar);
}

void MainWindow::loadMediaData(const QString &filePath)
{
    if (mediaService->loadFromFile(filePath)) {
        currentJsonPath = filePath;
        updateMediaDisplay();
    }
}

void MainWindow::updateMediaDisplay()
{
    QVector<Media*> filteredMedia = mediaService->filterMedia(currentCategory, searchBar->text());
    rightLayoutWidget->setMediaCollection(filteredMedia);
    rightLayoutWidget->setJsonService(jsonService);
    rightLayoutWidget->displayMediaCollection();
}

void MainWindow::handleUploadRequest()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Seleziona file JSON"),
        QDir::homePath(),
        tr("File JSON (*.json)")
    );
    
    if (!filePath.isEmpty()) {
        loadMediaData(filePath);
        showMessage("File caricato correttamente!");
    }
}

void MainWindow::handleExportRequest()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Esporta biblioteca in JSON"),
        QDir::homePath() + "/biblioteca_export.json",
        tr("File JSON (*.json)")
    );
    
    if (filePath.isEmpty()) return;
    
    if (!filePath.endsWith(".json", Qt::CaseInsensitive)) {
        filePath += ".json";
    }
    
    if (mediaService->saveToFile(filePath)) {
        showMessage("Biblioteca esportata correttamente in:\n" + QDir::toNativeSeparators(filePath));
        
        if (QMessageBox::question(this, "Apri file esportato",
                "Vuoi aprire il file appena esportato come biblioteca corrente?",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
            loadMediaData(filePath);
            showMessage("File caricato come biblioteca corrente!");
        }
    } else {
        showMessage("Impossibile esportare i dati in:\n" + QDir::toNativeSeparators(filePath) +
                   "\n\nAssicurati di avere i permessi di scrittura per questa posizione.", 
                   MessageType::ERROR);
    }
}

void MainWindow::showCreateItemWidget()
{
    createItemWidget->resetToCreateMode();
    rightLayoutWidget->showCreateItemWidget(createItemWidget);
}

void MainWindow::onMediaItemCreated(Media* newItem)
{
    if (!newItem) return;
    
    handleMediaOperation(
        [this, newItem]() { return mediaService->addMedia(newItem) && saveCurrentData(); },
        "Media creato e salvato correttamente!\n" + QString::fromStdString(newItem->getTitolo()) + " è stato aggiunto alla biblioteca.",
        "Media creato ma non è stato possibile salvarlo automaticamente.\nUsa il pulsante 'Salva' per salvare manualmente i dati."
    );
}

void MainWindow::onSearchTextChanged(const QString& text)
{
    Q_UNUSED(text)
    updateMediaDisplay();
}

void MainWindow::onMediaEditRequested(Media* media)
{
    if (!media) return;
    
    rightLayoutWidget->showCreateItemWidget(createItemWidget);
    createItemWidget->setEditMode(media);
}

void MainWindow::onMediaDeleteRequested(Media* media)
{
    if (!media) return;
    
    if (QMessageBox::question(this, "Conferma Eliminazione",
            QString("Sei sicuro di voler eliminare '%1'?\n\nQuesta operazione non può essere annullata.")
                .arg(QString::fromStdString(media->getTitolo())),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) {
        return;
    }
    
    QString mediaTitle = QString::fromStdString(media->getTitolo());
    
    handleMediaOperation(
        [this, media]() { return mediaService->deleteMedia(media) && saveCurrentData(); },
        QString("'%1' è stato eliminato correttamente dalla biblioteca.").arg(mediaTitle),
        "Impossibile salvare le modifiche.\nIl media è stato rimosso dalla visualizzazione ma la biblioteca non potrebbe essere aggiornata."
    );
}

void MainWindow::onMediaItemUpdated(Media* oldMedia, Media* newMedia)
{
    if (!oldMedia || !newMedia) return;
    
    handleMediaOperation(
        [this, oldMedia, newMedia]() { return mediaService->updateMedia(oldMedia, newMedia) && saveCurrentData(); },
        QString("'%1' è stato aggiornato").arg(QString::fromStdString(newMedia->getTitolo())),
        "Impossibile salvare le modifiche.\nIl media è stato aggiornato nella visualizzazione ma la biblioteca non potrebbe essere aggiornata."
    );
}

void MainWindow::handleCloseRequest()
{
    if (QMessageBox::question(this, "Chiudi Biblioteca",
            "Sei sicuro di voler chiudere la biblioteca corrente?\n",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        clearCurrentLibrary();
        showMessage("La biblioteca è stata chiusa correttamente.");
    }
}

void MainWindow::clearCurrentLibrary()
{
    mediaService->clearMediaCollection();
    currentJsonPath = "";
    currentCategory = "Tutti";
    searchBar->clear();
    updateCategoryButtonStates("Tutti");
    rightLayoutWidget->setMediaCollection(QVector<Media*>());
    rightLayoutWidget->displayMediaCollection();
}

void MainWindow::handleThemeToggle()
{
    StyleUtils::toggleTheme();
    topMenu->updateThemeButtonIcon();
    refreshAllStyles();
}

// Per aggiornare gli stili di tutti i componenti UI, implementato dopo l'aggiunta deel tema scuro

void MainWindow::refreshAllStyles()
{
    // Aggiorna gli stili principali
    this->setStyleSheet(StyleUtils::getMainWindowStyle());
    leftWidget->setStyleSheet(StyleUtils::getSidebarStyle());
    
    rightScrollArea->setStyleSheet("");
    rightScrollArea->setStyleSheet(StyleUtils::getScrollAreaStyle());
    
    rightLayoutWidget->refreshStyles();
    
    QWidget* centralWidget = this->centralWidget();
    if (centralWidget) {
        centralWidget->setStyleSheet("");
        centralWidget->setStyleSheet(StyleUtils::getContentAreaStyle());
    }
    
    // Aggiorna i bottoni di categoria
    for (auto it = categoryButtons.begin(); it != categoryButtons.end(); ++it) {
        it.value()->setStyleSheet(StyleUtils::getCategoryButtonStyle());
    }
    
    // Aggiorna la barra di ricerca
    searchBar->setStyleSheet(StyleUtils::getSearchBarStyle());
    
    // Aggiorna il top menu
    topMenu->setStyleSheet("");
    topMenu->setStyleSheet(StyleUtils::getSidebarStyle());
    topMenu->updateThemeButtonIcon();
    
    QList<QPushButton*> topMenuButtons = topMenu->findChildren<QPushButton*>();
    for (QPushButton* button : topMenuButtons) {
        button->setStyleSheet(StyleUtils::getTopButtonStyle());
    }
    
    // Aggiorna il CreateItemWidget se è visibile
    if (createItemWidget) {
        createItemWidget->refreshStyles();
        // Aggiorna lo stile del widget principale
        createItemWidget->setStyleSheet("");
        createItemWidget->setStyleSheet(StyleUtils::getContentAreaStyle());
        
        // Aggiorna tutti i ComboBox
        QList<QComboBox*> comboBoxes = createItemWidget->findChildren<QComboBox*>();
        for (QComboBox* combo : comboBoxes) {
            combo->setStyleSheet(StyleUtils::getComboBoxStyle());
        }
        
        // Aggiorna tutti i QLineEdit
        QList<QLineEdit*> lineEdits = createItemWidget->findChildren<QLineEdit*>();
        for (QLineEdit* lineEdit : lineEdits) {
            lineEdit->setStyleSheet(StyleUtils::getLineEditStyle());
        }
        
        // Aggiorna tutti i QPushButton
        QList<QPushButton*> buttons = createItemWidget->findChildren<QPushButton*>();
        for (QPushButton* button : buttons) {
            if (button->objectName() == "createButton" || button->text().contains("Crea") || button->text().contains("Aggiorna")) {
                if (button->text().contains("Aggiorna")) {
                    button->setStyleSheet(StyleUtils::getWarningButtonStyle());
                } else {
                    button->setStyleSheet(StyleUtils::getPrimaryButtonStyle());
                }
            } else {
                button->setStyleSheet(StyleUtils::getItemButtonStyle());
            }
        }
        
        // Aggiorna tutti i QLabel
        QList<QLabel*> labels = createItemWidget->findChildren<QLabel*>();
        for (QLabel* label : labels) {
            label->setStyleSheet(StyleUtils::getSubtitleLabelStyle());
        }
    }
    
    updateMediaDisplay();
    repaint();
    update();
}

bool MainWindow::saveCurrentData()
{
    return !currentJsonPath.isEmpty() && mediaService->saveToFile(currentJsonPath);
}

void MainWindow::showMessage(const QString& message, MessageType type)
{
    switch (type) {
        case MessageType::SUCCESS:
            QMessageBox::information(this, "Successo", message);
            break;
        case MessageType::WARNING:
            QMessageBox::warning(this, "Attenzione", message);
            break;
        case MessageType::ERROR:
            QMessageBox::critical(this, "Errore", message);
            break;
    }
}

bool MainWindow::handleMediaOperation(const std::function<bool()>& operation, 
                                     const QString& successMsg, 
                                     const QString& errorMsg)
{
    if (operation()) {
        showMessage(successMsg);
        updateMediaDisplay();
        return true;
    } else {
        showMessage(errorMsg, MessageType::ERROR);
        return false;
    }
}