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
    setWindowTitle("Event Manager");

    setupUI();
    initializeServices();
    setupConnections();
    loadDefaultData();
}

MainWindow::~MainWindow()
{
    delete eventService;
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
    mainLayout->addWidget(rightPanelContainer, 1);

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
    // Container per tutto il pannello destro
    rightPanelContainer = new QWidget();
    QVBoxLayout* rightPanelLayout = new QVBoxLayout(rightPanelContainer);
    rightPanelLayout->setContentsMargins(0, 0, 0, 0);
    rightPanelLayout->setSpacing(0);
    
    // Header toolbar in alto a destra
    headerToolbar = new HeaderToolbarWidget(this);
    rightPanelLayout->addWidget(headerToolbar);
    
    // Stack per le diverse viste
    viewStack = new QStackedWidget();
    
    // Vista lista (esistente)
    rightLayoutWidget = new RightLayoutWidget(this);
    rightScrollArea = new QScrollArea(this);
    rightScrollArea->setWidgetResizable(true);
    rightScrollArea->setWidget(rightLayoutWidget);
    rightScrollArea->setFrameShape(QFrame::NoFrame);
    rightScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // Vista calendario (nuova)
    calendarViewWidget = new CalendarViewWidget(this);
    
    viewStack->addWidget(rightScrollArea);      // index 0 = lista
    viewStack->addWidget(calendarViewWidget);   // index 1 = calendario
    
    rightPanelLayout->addWidget(viewStack, 1);
}

void MainWindow::initializeServices()
{
    jsonService = new JsonService(this);
    eventService = new EventService(this);
    eventService->setJsonService(jsonService);
    
    uiService = new UIService(this);
    rightLayoutWidget->setUIService(uiService);
    calendarViewWidget->setUIService(uiService);

    createItemWidget = new CreateItemWidget(this);
    createItemWidget->setObjectName("CreateItemWidget");
    createItemWidget->setEventService(eventService);
    createItemWidget->hide();
}

void MainWindow::setupConnections()
{
    // TopMenu connections
    connect(topMenu, &TopMenuWidget::uploadRequested, this, &MainWindow::handleUploadRequest);
    connect(topMenu, &TopMenuWidget::createRequested, this, &MainWindow::showCreateItemWidget);
    connect(topMenu, &TopMenuWidget::exportRequested, this, &MainWindow::handleExportRequest);
    connect(topMenu, &TopMenuWidget::closeRequested, this, &MainWindow::handleCloseRequest);
    
    // Header toolbar connections
    connect(headerToolbar, &HeaderToolbarWidget::themeToggleRequested, this, &MainWindow::handleThemeToggle);
    connect(headerToolbar, &HeaderToolbarWidget::calendarViewRequested, this, &MainWindow::showCalendarView);
    connect(headerToolbar, &HeaderToolbarWidget::listViewRequested, this, &MainWindow::showListView);
    
    // CreateItemWidget connections
    connect(createItemWidget, &CreateItemWidget::itemCreated, this, &MainWindow::onEventItemCreated);
    connect(createItemWidget, &CreateItemWidget::itemUpdated, this, &MainWindow::onEventItemUpdated);
    
    // RightLayoutWidget connections
    connect(rightLayoutWidget, &RightLayoutWidget::eventEditRequested, this, &MainWindow::onEventEditRequested);
    connect(rightLayoutWidget, &RightLayoutWidget::eventDeleteRequested, this, &MainWindow::onEventDeleteRequested);
    
    // CalendarViewWidget connections
    connect(calendarViewWidget, &CalendarViewWidget::eventEditRequested, this, &MainWindow::onEventEditRequested);
    connect(calendarViewWidget, &CalendarViewWidget::eventDeleteRequested, this, &MainWindow::onEventDeleteRequested);
}

void MainWindow::loadDefaultData()
{
    if (QFile::exists(currentJsonPath)) {
        loadEventData(currentJsonPath);
    }
}

void MainWindow::setupCategoryButtons()
{
    static const QStringList categories = {"Tutti", "Appointment", "Deadline", "Recursive", "Reminder"};

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
    
    QVector<Event*> filteredEvents = eventService->filterEvents(category, searchBar->text());
    
    // Aggiorna entrambe le viste
    rightLayoutWidget->setEventCollection(filteredEvents);
    rightLayoutWidget->setJsonService(jsonService);
    rightLayoutWidget->displayEventCollection();
    
    calendarViewWidget->setEventCollection(filteredEvents);

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
    searchBar->setPlaceholderText("Cerca per nome...");
    searchBar->setClearButtonEnabled(true);
    searchBar->setStyleSheet(StyleUtils::getSearchBarStyle());
    searchBar->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);

    connect(searchBar, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    leftLayout->addWidget(searchBar);
}

void MainWindow::loadEventData(const QString &filePath)
{
    if (eventService->loadFromFile(filePath)) {
        currentJsonPath = filePath;
        updateEventDisplay();
    }
}

void MainWindow::updateEventDisplay()
{
    QVector<Event*> filteredEvents = eventService->filterEvents(currentCategory, searchBar->text());
    
    // Aggiorna entrambe le viste
    rightLayoutWidget->setEventCollection(filteredEvents);
    rightLayoutWidget->setJsonService(jsonService);
    rightLayoutWidget->displayEventCollection();
    
    calendarViewWidget->setEventCollection(filteredEvents);
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
        loadEventData(filePath);
        showMessage("File caricato correttamente!");
    }
}

void MainWindow::handleExportRequest()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Esporta calendario in JSON"),
        QDir::homePath() + "/calendario_export.json",
        tr("File JSON (*.json)")
    );
    
    if (filePath.isEmpty()) return;
    
    if (!filePath.endsWith(".json", Qt::CaseInsensitive)) {
        filePath += ".json";
    }
    
    if (eventService->saveToFile(filePath)) {
        showMessage("Calendario esportato correttamente in:\n" + QDir::toNativeSeparators(filePath));
        
        if (QMessageBox::question(this, "Apri file esportato",
                "Vuoi aprire il file appena esportato come calendario corrente?",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
            loadEventData(filePath);
            showMessage("File caricato come calendario corrente!");
        }
    } else {
        showMessage("Impossibile esportare i dati in:\n" + QDir::toNativeSeparators(filePath) +
                   "\n\nAssicurati di avere i permessi di scrittura per questa posizione.", 
                   MessageType::ERROR);
    }
}

void MainWindow::showCreateItemWidget()
{
    // Assicurati di essere nella vista lista
    if (currentView != ViewType::LIST) {
        showListView();
    }
    
    createItemWidget->resetToCreateMode();
    rightLayoutWidget->showCreateItemWidget(createItemWidget);
}

void MainWindow::onEventItemCreated(Event* newItem)
{
    if (!newItem) return;
    
    handleEventOperation(
        [this, newItem]() { return eventService->addEvent(newItem) && saveCurrentData(); },
        "Evento creato e salvato correttamente!\n" + QString::fromStdString(newItem->getName()) + " è stato aggiunto al calendario.",
        "Evento creato ma non è stato possibile salvarlo automaticamente.\nUsa il pulsante 'Salva' per salvare manualmente i dati."
    );
}

void MainWindow::onSearchTextChanged(const QString& text)
{
    Q_UNUSED(text)
    updateEventDisplay();
}

void MainWindow::onEventEditRequested(Event* event)
{
    if (!event) return;
    
    // Passa alla vista lista per l'editing
    if (currentView != ViewType::LIST) {
        showListView();
    }
    
    rightLayoutWidget->showCreateItemWidget(createItemWidget);
    createItemWidget->setEditMode(event);
}

void MainWindow::onEventDeleteRequested(Event* event)
{
    if (!event) return;
    
    if (QMessageBox::question(this, "Conferma Eliminazione",
            QString("Sei sicuro di voler eliminare '%1'?\n\nQuesta operazione non può essere annullata.")
                .arg(QString::fromStdString(event->getName())),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) {
        return;
    }
    
    QString eventName = QString::fromStdString(event->getName());
    
    handleEventOperation(
        [this, event]() { return eventService->deleteEvent(event) && saveCurrentData(); },
        QString("'%1' è stato eliminato correttamente dal calendario.").arg(eventName),
        "Impossibile salvare le modifiche.\nL'evento è stato rimosso dalla visualizzazione ma il calendario potrebbe non essere aggiornato."
    );
}

void MainWindow::onEventItemUpdated(Event* oldEvent, Event* newEvent)
{
    if (!oldEvent || !newEvent) return;
    
    handleEventOperation(
        [this, oldEvent, newEvent]() { return eventService->updateEvent(oldEvent, newEvent) && saveCurrentData(); },
        QString("'%1' è stato aggiornato").arg(QString::fromStdString(newEvent->getName())),
        "Impossibile salvare le modifiche.\nL'evento è stato aggiornato nella visualizzazione ma il calendario potrebbe non essere aggiornato."
    );
}

void MainWindow::handleCloseRequest()
{
    if (QMessageBox::question(this, "Chiudi Calendario",
            "Sei sicuro di voler chiudere il calendario corrente?\n",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        clearCurrentLibrary();
        showMessage("Il calendario è stato chiuso correttamente.");
    }
}

void MainWindow::clearCurrentLibrary()
{
    eventService->clearEventCollection();
    currentJsonPath = "";
    currentCategory = "Tutti";
    searchBar->clear();
    updateCategoryButtonStates("Tutti");
    rightLayoutWidget->setEventCollection(QVector<Event*>());
    rightLayoutWidget->displayEventCollection();
    calendarViewWidget->setEventCollection(QVector<Event*>());
}

void MainWindow::handleThemeToggle()
{
    StyleUtils::toggleTheme();
    headerToolbar->updateThemeButtonIcon();
    refreshAllStyles();
}

void MainWindow::showCalendarView()
{
    switchToView(ViewType::CALENDAR);
}

void MainWindow::showListView()
{
    switchToView(ViewType::LIST);
}

void MainWindow::switchToView(ViewType view)
{
    currentView = view;
    
    if (view == ViewType::CALENDAR) {
        viewStack->setCurrentIndex(1);
        headerToolbar->setCalendarViewActive(true);
    } else {
        viewStack->setCurrentIndex(0);
        headerToolbar->setCalendarViewActive(false);
    }
}

void MainWindow::refreshAllStyles()
{
    // Aggiorna gli stili principali
    this->setStyleSheet(StyleUtils::getMainWindowStyle());
    leftWidget->setStyleSheet(StyleUtils::getSidebarStyle());
    
    rightScrollArea->setStyleSheet("");
    rightScrollArea->setStyleSheet(StyleUtils::getScrollAreaStyle());
    
    rightLayoutWidget->refreshStyles();
    calendarViewWidget->refreshStyles();
    headerToolbar->refreshStyles();
    
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
    
    QList<QPushButton*> topMenuButtons = topMenu->findChildren<QPushButton*>();
    for (QPushButton* button : topMenuButtons) {
        button->setStyleSheet(StyleUtils::getTopButtonStyle());
    }
    
    // Aggiorna il CreateItemWidget se è visibile
    if (createItemWidget) {
        createItemWidget->refreshStyles();
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
    
    updateEventDisplay();
    repaint();
    update();
}

bool MainWindow::saveCurrentData()
{
    return !currentJsonPath.isEmpty() && eventService->saveToFile(currentJsonPath);
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

bool MainWindow::handleEventOperation(const std::function<bool()>& operation, 
                                     const QString& successMsg, 
                                     const QString& errorMsg)
{
    if (operation()) {
        showMessage(successMsg);
        updateEventDisplay();
        return true;
    } else {
        showMessage(errorMsg, MessageType::ERROR);
        return false;
    }
}