
#include "headerToolbarWidget.h"
#include "../../services/styleUtils.h"

HeaderToolbarWidget::HeaderToolbarWidget(QWidget* parent) 
    : QWidget(parent), isCalendarView(false)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(StyleUtils::Dimensions::PADDING_MEDIUM, 
                              StyleUtils::Dimensions::PADDING_MEDIUM, 
                              StyleUtils::Dimensions::PADDING_MEDIUM, 
                              StyleUtils::Dimensions::PADDING_MEDIUM);
    layout->setSpacing(StyleUtils::Dimensions::SPACING_MEDIUM);

    // Pulsante Toggle Vista (Lista/Calendario)
    viewToggleButton = new QPushButton(this);
    viewToggleButton->setObjectName("viewToggleButton");
    viewToggleButton->setIcon(QIcon("../resources/icon/calendar.png"));
    viewToggleButton->setIconSize(QSize(StyleUtils::Dimensions::ICON_SIZE_LARGE, 
                                        StyleUtils::Dimensions::ICON_SIZE_LARGE));
    viewToggleButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);
    viewToggleButton->setToolTip("Passa alla vista calendario");
    viewToggleButton->setStyleSheet(StyleUtils::getTopButtonStyle());

    // Pulsante Theme Toggle
    themeToggleButton = new QPushButton(this);
    themeToggleButton->setObjectName("themeToggleButton");
    themeToggleButton->setIconSize(QSize(StyleUtils::Dimensions::ICON_SIZE_LARGE, 
                                        StyleUtils::Dimensions::ICON_SIZE_LARGE));
    themeToggleButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);
    themeToggleButton->setToolTip("Cambia tema (chiaro/scuro)");
    themeToggleButton->setStyleSheet(StyleUtils::getTopButtonStyle());
    updateThemeButtonIcon();

    // Connessioni
    connect(viewToggleButton, &QPushButton::clicked, this, [this]() {
        if (isCalendarView) {
            emit listViewRequested();
        } else {
            emit calendarViewRequested();
        }
    });
    
    connect(themeToggleButton, &QPushButton::clicked, this, &HeaderToolbarWidget::themeToggleRequested);

    layout->addStretch();
    layout->addWidget(viewToggleButton);
    layout->addWidget(themeToggleButton);

    setLayout(layout);
}

void HeaderToolbarWidget::updateThemeButtonIcon()
{
    if (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) {
        themeToggleButton->setIcon(QIcon("../resources/icon/moon.png"));
    } else {
        themeToggleButton->setIcon(QIcon("../resources/icon/sun.png"));
    }
}

void HeaderToolbarWidget::setCalendarViewActive(bool active)
{
    isCalendarView = active;
    if (isCalendarView) {
        viewToggleButton->setIcon(QIcon("../resources/icon/list.png"));
        viewToggleButton->setToolTip("Passa alla vista lista");
    } else {
        viewToggleButton->setIcon(QIcon("../resources/icon/calendar.png"));
        viewToggleButton->setToolTip("Passa alla vista calendario");
    }
}

void HeaderToolbarWidget::refreshStyles()
{
    viewToggleButton->setStyleSheet(StyleUtils::getTopButtonStyle());
    themeToggleButton->setStyleSheet(StyleUtils::getTopButtonStyle());
    updateThemeButtonIcon();
}