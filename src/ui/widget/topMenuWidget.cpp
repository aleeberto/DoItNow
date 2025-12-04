#include "topMenuWidget.h"
#include <QHBoxLayout>
#include "../../services/styleUtils.h"

TopMenuWidget::TopMenuWidget(QWidget* parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(StyleUtils::Dimensions::PADDING_LARGE, 
                              StyleUtils::Dimensions::PADDING_LARGE, 
                              StyleUtils::Dimensions::PADDING_LARGE, 
                              StyleUtils::Dimensions::PADDING_LARGE);
    layout->setSpacing(StyleUtils::Dimensions::SPACING_MEDIUM);

    // Pulsante Upload
    QPushButton* uploadButton = new QPushButton(this);
    uploadButton->setIcon(QIcon("../resources/icon/open.png"));
    uploadButton->setIconSize(QSize(StyleUtils::Dimensions::ICON_SIZE_LARGE, 
                                   StyleUtils::Dimensions::ICON_SIZE_LARGE));
    uploadButton->setToolTip("Carica una biblioteca esistente");
    uploadButton->setStyleSheet(StyleUtils::getTopButtonStyle());
    uploadButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);

    // Pulsante Export 
    QPushButton* exportButton = new QPushButton(this);
    exportButton->setIcon(QIcon("../resources/icon/create.png"));
    exportButton->setIconSize(QSize(StyleUtils::Dimensions::ICON_SIZE_LARGE, 
                                   StyleUtils::Dimensions::ICON_SIZE_LARGE));
    exportButton->setToolTip("Crea una nuova biblioteca");
    exportButton->setStyleSheet(StyleUtils::getTopButtonStyle());
    exportButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);

    // Pulsante Close
    QPushButton* closeButton = new QPushButton(this);
    closeButton->setIcon(QIcon("../resources/icon/close.png"));
    closeButton->setIconSize(QSize(StyleUtils::Dimensions::ICON_SIZE_LARGE, 
                                  StyleUtils::Dimensions::ICON_SIZE_LARGE));
    closeButton->setToolTip("Chiude la biblioteca corrente");
    closeButton->setStyleSheet(StyleUtils::getTopButtonStyle());
    closeButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);

    // Pulsante Create
    QPushButton* createButton = new QPushButton(this);
    createButton->setIcon(QIcon("../resources/icon/add.png"));
    createButton->setIconSize(QSize(StyleUtils::Dimensions::ICON_SIZE_LARGE, 
                                   StyleUtils::Dimensions::ICON_SIZE_LARGE));
    createButton->setToolTip("Crea un nuovo elemento");
    createButton->setStyleSheet(StyleUtils::getTopButtonStyle());
    createButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);

    // Pulsante Theme Toggle
    themeToggleButton = new QPushButton(this);
    themeToggleButton->setObjectName("themeToggleButton");
    themeToggleButton->setIconSize(QSize(StyleUtils::Dimensions::ICON_SIZE_LARGE, 
                                        StyleUtils::Dimensions::ICON_SIZE_LARGE));
    themeToggleButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);
    themeToggleButton->setToolTip("Cambia tema (chiaro/scuro)");
    themeToggleButton->setStyleSheet(StyleUtils::getTopButtonStyle());
    updateThemeButtonIcon();

    // Connessioni dei pulsanti
    connect(uploadButton, &QPushButton::clicked, this, &TopMenuWidget::uploadRequested);
    connect(exportButton, &QPushButton::clicked, this, &TopMenuWidget::exportRequested);
    connect(closeButton, &QPushButton::clicked, this, &TopMenuWidget::closeRequested);
    connect(createButton, &QPushButton::clicked, this, &TopMenuWidget::createRequested);
    connect(themeToggleButton, &QPushButton::clicked, this, &TopMenuWidget::themeToggleRequested);

    layout->addWidget(uploadButton);
    layout->addWidget(exportButton);
    layout->addWidget(closeButton);
    layout->addWidget(createButton);
    layout->addStretch(); // Allontana se ingrandita la finestra
    layout->addWidget(themeToggleButton);

    this->setLayout(layout);
}

// Cambio icona pulsante toggle

void TopMenuWidget::updateThemeButtonIcon() {
    if (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) {
        themeToggleButton->setIcon(QIcon("../resources/icon/moon.png"));
    } else {
        themeToggleButton->setIcon(QIcon("../resources/icon/sun.png"));
    }
}