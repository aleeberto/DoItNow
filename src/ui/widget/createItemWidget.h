#ifndef CREATEITEMWIDGET_HEADER
#define CREATEITEMWIDGET_HEADER

#include <QWidget>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QResizeEvent>
#include <QMessageBox>
#include <QMap>
#include "../../logic/media.h"
#include "../../services/mediaService.h"

// Forward declaration
class FormWidgetVisitor;

class CreateItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CreateItemWidget(QWidget *parent = nullptr);
    
    // Metodi principali
    Media* createMediaItem() const;
    void setMediaService(MediaService* service);
    
    // Gestione modalità, e stile
    void setEditMode(Media* media);
    void resetToCreateMode();
    bool isInEditMode() const;
    void refreshStyles();

signals:
    void itemCreated(Media* newItem);
    void itemUpdated(Media* oldMedia, Media* newMedia);

public slots:
    void onItemTypeChanged(int index);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onCreateButtonClicked();

private:
    // Setup methods
    void setupTypeSelection();
    void updateScrollBarVisibility();
    
    // Edit mode helpers
    void populateFields(Media* media, const QString& mediaType);
    void populateCommonFields(const QList<QLineEdit*>& fields, Media* media);
    void populateSeriesFields(const QList<QLineEdit*>& fields, int numEpisodi, int numStagioni, 
                             int durataMedia, bool inCorso, const std::string& creator, 
                             const std::string& company);
    void clearEditMode();
    void clearAllFields();

    // UI Components
    QComboBox *itemTypeCombo;
    QStackedWidget *stackedFields;
    QPushButton *createButton;
    QScrollArea *scrollArea;
        
    // Services
    MediaService* mediaService;
    FormWidgetVisitor* formVisitor;
    
    // Widget
    QMap<QString, QWidget*> typeWidgets;

    // Edit mode state
    Media* editingMedia;
    bool editMode;

};

#endif // CREATEITEMWIDGET_HEADER