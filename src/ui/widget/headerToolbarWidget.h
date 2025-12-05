#ifndef HEADERTOOLBARWIDGET_HEADER
#define HEADERTOOLBARWIDGET_HEADER

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>

class HeaderToolbarWidget : public QWidget {
    Q_OBJECT
public:
    explicit HeaderToolbarWidget(QWidget* parent = nullptr);
    
    void updateThemeButtonIcon();
    void setCalendarViewActive(bool active);
    void refreshStyles();
    
signals:
    void themeToggleRequested();
    void calendarViewRequested();
    void listViewRequested();

private:
    QPushButton* themeToggleButton;
    QPushButton* viewToggleButton;
    bool isCalendarView;
};

#endif // HEADERTOOLBARWIDGET_HEADER