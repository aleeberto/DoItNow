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
#include <QStackedWidget>
#include "widget/topMenuWidget.h"
#include "widget/createItemWidget.h"
#include "widget/rightLayoutWidget.h"
#include "widget/calendarViewWidget.h"
#include "widget/headerToolbarWidget.h"
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
    
    // Slot per cambio vista
    void showCalendarView();
    void showListView();

private:
    // Enum per messaggi unificato
    enum class MessageType {
        SUCCESS,
        WARNING,
        ERROR
    };
    
    // Enum per tipo di vista
    enum class ViewType {
        LIST,
        CALENDAR
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
    
    // View management
    void switchToView(ViewType view);

    // Message helper unificato
    void showMessage(const QString& message, MessageType type = MessageType::SUCCESS);
    
    // Helper per operazioni con eventi
    bool handleEventOperation(const std::function<bool()>& operation, 
                             const QString& successMsg, 
                             const QString& errorMsg = "Operazione fallita");

    // UI Components
    QWidget* leftWidget;
    QWidget* rightPanelContainer;
    QScrollArea* rightScrollArea;
    QStackedWidget* viewStack;
    TopMenuWidget* topMenu;
    HeaderToolbarWidget* headerToolbar;
    CreateItemWidget* createItemWidget;
    RightLayoutWidget* rightLayoutWidget;
    CalendarViewWidget* calendarViewWidget;
    QVBoxLayout* leftLayout;
    QLineEdit* searchBar;
    QMap<QString, QPushButton*> categoryButtons;

    // Services
    JsonService* jsonService;
    EventService* eventService;
    UIService* uiService;

    // State
    QString currentJsonPath = "../resources/data/data.json";
    QString currentCategory = "Tutti";
    ViewType currentView = ViewType::LIST;
};

#endif // MAINWINDOW_HEADER