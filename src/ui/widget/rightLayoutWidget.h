#ifndef RIGHTLAYOUTWIDGET_HEADER
#define RIGHTLAYOUTWIDGET_HEADER

#include <QWidget>
#include <QVBoxLayout>
#include <QVector>
#include "../../logic/media.h"
#include "../../services/jsonService.h"
#include "../../services/uiService.h"

// Forward declaration
class MediaWidgetVisitor;

class RightLayoutWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RightLayoutWidget(QWidget *parent = nullptr);
    ~RightLayoutWidget();

    void setMediaCollection(const QVector<Media*>& collection);
    void setJsonService(JsonService* service);
    void setUIService(UIService* service);
    void displayMediaCollection();
    void clearLayout();
    void showCreateItemWidget(QWidget* createWidget);
    void refreshStyles();

signals:
    void mediaEditRequested(Media* media);
    void mediaDeleteRequested(Media* media);

private slots:
    void onEditButtonClicked();
    void onDeleteButtonClicked();

private:
    void addMediaCardToLayout(Media* media);

    QVBoxLayout *mainLayout;
    QVector<Media*> mediaCollection;
    JsonService *jsonService;
    UIService *uiService;
    MediaWidgetVisitor *widgetVisitor;
};

#endif // RIGHTLAYOUTWIDGET_HEADER