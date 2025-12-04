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
#include "../services/mediaService.h"
#include "../services/uiService.h"
#include "../logic/media.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slot principali
    void showCreateItemWidget();
    void onMediaItemCreated(Media* newItem);
    void onMediaItemUpdated(Media* oldMedia, Media* newMedia);
    void onMediaEditRequested(Media* media);
    void onMediaDeleteRequested(Media* media);
    void handleUploadRequest();
    void handleExportRequest();
    void handleCloseRequest();
    void handleThemeToggle();
    void onSearchTextChanged(const QString& text);

private:
    // Enum per messaggi unificato (success, warning, error) (check if working)
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
    void loadMediaData(const QString &filePath);
    void updateMediaDisplay();
    bool saveCurrentData();

    // Library management
    void clearCurrentLibrary();

    // Message helper unificato (check if working)
    void showMessage(const QString& message, MessageType type = MessageType::SUCCESS);
    
    // Helper per operazioni con media
    bool handleMediaOperation(const std::function<bool()>& operation, 
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
    MediaService* mediaService;
    UIService* uiService;

    // Initial state
    QString currentJsonPath = "../resources/data/data.json";
    QString currentCategory = "Tutti";
};

#endif // MAINWINDOW_HEADER