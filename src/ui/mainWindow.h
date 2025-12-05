#ifndef MAINWINDOW_HEADER
#define MAINWINDOW_HEADER

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QMessageBox>
#include "widget/topMenuWidget.h"
#include "widget/createItemWidget.h"
#include "widget/rightLayoutWidget.h"
#include "../services/jsonService.h"
#include "../services/eventService.h"
#include "../services/uiService.h"
#include "../logic/event.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slot principali
    void showCreateItemWidget();
    void onEventItemCreated(Event* newItem);
    void onEventItemUpdated(Event* oldEvent, Event* newEvent);
    void onEventEditRequested(Event* event);
    void onEventDeleteRequested(Event* event);
    void handleUploadRequest();
    void handleExportRequest();
    void handleCloseRequest();
    void handleThemeToggle();
    void onSearchTextChanged(const QString& text);

private:
    // Enum per messaggi unificato
    enum class MessageType {
        SUCCESS,
        WARNING,
        ERROR
    };

    // UI Setup methods
    void setupUI();
    void setupLeftPanel();
    void setupRightPanel();
    void setupCategoryButtons();
    void setupSearchBar();
    void initializeServices();
    void setupConnections();
    void loadDefaultData();

    // Category management
    QPushButton* createCategoryButton(const QString& category);
    void handleCategorySelection(const QString& category);
    void updateCategoryButtonStates(const QString& selectedCategory);

    // Theme management
    void refreshAllStyles();

    // Data management
    void loadEventData(const QString &filePath);
    void updateEventDisplay();
    bool saveCurrentData();

    // Library management
    void clearCurrentLibrary();

    // Message helper unificato
    void showMessage(const QString& message, MessageType type = MessageType::SUCCESS);
    
    // Helper per operazioni con eventi
    bool handleEventOperation(const std::function<bool()>& operation, 
                             const QString& successMsg, 
                             const QString& errorMsg = "Operazione fallita");

    // UI Components
    QWidget* leftWidget;
    QScrollArea* rightScrollArea;
    TopMenuWidget* topMenu;
    CreateItemWidget* createItemWidget;
    RightLayoutWidget* rightLayoutWidget;
    QVBoxLayout* leftLayout;
    QLineEdit* searchBar;
    QMap<QString, QPushButton*> categoryButtons;

    // Services
    JsonService* jsonService;
    EventService* eventService;
    UIService* uiService;

    // Initial state
    QString currentJsonPath = "../resources/data/data.json";
    QString currentCategory = "Tutti";
};

#endif // MAINWINDOW_HEADER