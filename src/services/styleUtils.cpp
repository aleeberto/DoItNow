#include "styleUtils.h"

// Inizializzazione del tema corrente
StyleUtils::Theme StyleUtils::currentTheme = StyleUtils::Theme::LIGHT;

// Gestione del tema
void StyleUtils::setTheme(Theme theme) {
    currentTheme = theme;
}

StyleUtils::Theme StyleUtils::getCurrentTheme() {
    return currentTheme;
}

void StyleUtils::toggleTheme() {
    currentTheme = (currentTheme == Theme::LIGHT) ? Theme::DARK : Theme::LIGHT;
}

// Implementazione dei getter dinamici per i colori
const char* StyleUtils::Colors::PRIMARY() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_PRIMARY : DARK_PRIMARY;
}

const char* StyleUtils::Colors::PRIMARY_HOVER() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_PRIMARY_HOVER : DARK_PRIMARY_HOVER;
}

const char* StyleUtils::Colors::PRIMARY_PRESSED() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_PRIMARY_PRESSED : DARK_PRIMARY_PRESSED;
}

const char* StyleUtils::Colors::SECONDARY() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_SECONDARY : DARK_SECONDARY;
}

const char* StyleUtils::Colors::ACCENT() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_ACCENT : DARK_ACCENT;
}

const char* StyleUtils::Colors::BACKGROUND() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_BACKGROUND : DARK_BACKGROUND;
}

const char* StyleUtils::Colors::SURFACE() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_SURFACE : DARK_SURFACE;
}

const char* StyleUtils::Colors::TEXT() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_TEXT : DARK_TEXT;
}

const char* StyleUtils::Colors::TEXT_SECONDARY() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_TEXT_SECONDARY : DARK_TEXT_SECONDARY;
}

const char* StyleUtils::Colors::BORDER() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_BORDER : DARK_BORDER;
}

const char* StyleUtils::Colors::DANGER() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_DANGER : DARK_DANGER;
}

const char* StyleUtils::Colors::DANGER_HOVER() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_DANGER_HOVER : DARK_DANGER_HOVER;
}

const char* StyleUtils::Colors::DANGER_PRESSED() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_DANGER_PRESSED : DARK_DANGER_PRESSED;
}

const char* StyleUtils::Colors::WARNING() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_WARNING : DARK_WARNING;
}

const char* StyleUtils::Colors::WARNING_HOVER() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_WARNING_HOVER : DARK_WARNING_HOVER;
}

const char* StyleUtils::Colors::WARNING_PRESSED() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_WARNING_PRESSED : DARK_WARNING_PRESSED;
}

const char* StyleUtils::Colors::DISABLED_BG() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_DISABLED_BG : DARK_DISABLED_BG;
}

const char* StyleUtils::Colors::DISABLED_TEXT() {
    return (StyleUtils::getCurrentTheme() == StyleUtils::Theme::LIGHT) ? LIGHT_DISABLED_TEXT : DARK_DISABLED_TEXT;
}

// Metodi helper
QString StyleUtils::createButtonStyle(const QString& bgColor, const QString& textColor,
                                     const QString& hoverColor, const QString& pressedColor,
                                     const QString& borderColor, bool withDisabled)
{
    QString style = QString(
        "QPushButton {"
        "  background-color: %1;"
        "  color: %2;"
        "  border: %3;"
        "  border-radius: %4px;"
        "  padding: %5px %6px;"
        "  font-weight: %7;"
        "  font-size: %8px;"
        "  min-height: %9px;"
        "}"
        "QPushButton:hover {"
        "  background-color: %10;"
        "}"
        "QPushButton:pressed {"
        "  background-color: %11;"
        "}"
    ).arg(bgColor, textColor)
     .arg(borderColor == "none" ? "none" : QString("2px solid %1").arg(borderColor))
     .arg(Dimensions::BORDER_RADIUS)
     .arg(Dimensions::PADDING_MEDIUM)
     .arg(Dimensions::PADDING_LARGE + 8) // 24px
     .arg(FontWeights::SEMIBOLD)
     .arg(FontSizes::NORMAL)
     .arg(20) // min-height interno
     .arg(hoverColor)
     .arg(pressedColor);

    if (withDisabled) {
        style += QString(
            "QPushButton:disabled {"
            "  background-color: %1;"
            "  color: %2;"
            "}"
        ).arg(Colors::DISABLED_BG(), Colors::DISABLED_TEXT());
    }

    return style;
}

QString StyleUtils::createInputStyle(const QString& bgColor, const QString& borderColor,
                                    const QString& textColor, const QString& focusColor,
                                    int borderRadius)
{
    return QString(
        "QLineEdit {"
        "  background-color: %1;"
        "  border: 2px solid %2;"
        "  border-radius: %3px;"
        "  padding: %4px %5px;"
        "  font-size: %6px;"
        "  color: %7;"
        "  min-height: %8px;"
        "}"
        "QLineEdit:focus {"
        "  border-color: %9;"
        "}"
        "QLineEdit:hover {"
        "  border-color: %10;"
        "}"
    ).arg(bgColor, borderColor)
     .arg(borderRadius)
     .arg(Dimensions::PADDING_MEDIUM)
     .arg(Dimensions::PADDING_LARGE)
     .arg(FontSizes::NORMAL)
     .arg(textColor)
     .arg(20) // min-height interno
     .arg(focusColor)
     .arg(Colors::SECONDARY());
}

// Sezione Pulsanti
QString StyleUtils::getPrimaryButtonStyle()
{
    return createButtonStyle(Colors::PRIMARY(), "white", Colors::PRIMARY_HOVER(), Colors::PRIMARY_PRESSED());
}

QString StyleUtils::getTopButtonStyle()
{
    return createButtonStyle("transparent", Colors::SECONDARY(), Colors::SURFACE(), Colors::BORDER(), Colors::BORDER(), false) +
           QString("QPushButton { padding: %1px %2px; }")
           .arg(Dimensions::PADDING_MEDIUM - 2)
           .arg(Dimensions::PADDING_LARGE + 6);
}

QString StyleUtils::getItemButtonStyle()
{
    return createButtonStyle("transparent", Colors::SECONDARY(), Colors::SURFACE(), Colors::BORDER(), Colors::BORDER(), false) +
           QString("QPushButton { padding: %1px %2px; }")
           .arg(Dimensions::PADDING_MEDIUM - 2)
           .arg(Dimensions::PADDING_LARGE + 6);
}

QString StyleUtils::getDangerButtonStyle()
{
    return createButtonStyle(Colors::DANGER(), "white", Colors::DANGER_HOVER(), Colors::DANGER_PRESSED());
}

QString StyleUtils::getWarningButtonStyle()
{
    return createButtonStyle(Colors::WARNING(), "white", Colors::WARNING_HOVER(), Colors::WARNING_PRESSED());
}

QString StyleUtils::getIconButtonStyle()
{
    return QString(
        "QPushButton {"
        "  background-color: %1;"
        "  border: 1px solid %2;"
        "  border-radius: %3px;"
        "  padding: %4px;"
        "  min-width: %5px;"
        "  min-height: %5px;"
        "}"
        "QPushButton:hover {"
        "  background-color: %6;"
        "  border-color: %7;"
        "}"
        "QPushButton:pressed {"
        "  background-color: %8;"
        "}"
    ).arg(Colors::BACKGROUND(), Colors::BORDER())
     .arg(Dimensions::BORDER_RADIUS)
     .arg(Dimensions::PADDING_SMALL)
     .arg(Dimensions::ICON_SIZE)
     .arg(Colors::SURFACE(), Colors::SECONDARY(), Colors::BORDER());
}

// Sezione Campi di Input
QString StyleUtils::getLineEditStyle()
{
    return createInputStyle(Colors::BACKGROUND(), Colors::BORDER(), Colors::TEXT(), Colors::PRIMARY());
}

QString StyleUtils::getSearchBarStyle()
{
    return createInputStyle(Colors::SURFACE(), Colors::BORDER(), Colors::TEXT(), Colors::PRIMARY(), Dimensions::BORDER_RADIUS_SEARCH) +
           QString("QLineEdit { padding: %1px %2px; min-height: %3px; }")
           .arg(Dimensions::PADDING_SMALL)
           .arg(Dimensions::PADDING_LARGE)
           .arg(16);
}

QString StyleUtils::getComboBoxStyle()
{
    return QString(
        "QComboBox {"
        "  background-color: %1;"
        "  border: 2px solid %2;"
        "  border-radius: %3px;"
        "  padding: %4px %5px;"
        "  font-size: %6px;"
        "  color: %7;"
        "  min-height: %8px;"
        "}"
        "QComboBox:focus {"
        "  border-color: %9;"
        "}"
        "QComboBox::drop-down {"
        "  border: none;"
        "  width: %10px;"
        "}"
        "QComboBox::down-arrow {"
        "  width: %11px;"
        "  height: %11px;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background-color: %12;"
        "  color: %13;"
        "  border: 1px solid %14;"
        "  selection-background-color: %15;"
        "}"
    ).arg(Colors::BACKGROUND(), Colors::BORDER())
     .arg(Dimensions::BORDER_RADIUS)
     .arg(Dimensions::PADDING_MEDIUM)
     .arg(Dimensions::PADDING_LARGE)
     .arg(FontSizes::NORMAL)
     .arg(Colors::TEXT())
     .arg(20) // min-height interno
     .arg(Colors::PRIMARY())
     .arg(30) // drop-down width
     .arg(12) // arrow size
     .arg(Colors::SURFACE(), Colors::TEXT(), Colors::BORDER(), Colors::PRIMARY());
}

// Sezione Stili Generali
QString StyleUtils::getMainWindowStyle()
{
    return QString(
        "QMainWindow {"
        "  background-color: %1;"
        "  color: %2;"
        "}"
        "QWidget {"
        "  font-family: 'Inter', 'Segoe UI', 'Roboto', sans-serif;"
        "}"
    ).arg(Colors::BACKGROUND(), Colors::TEXT());
}

QString StyleUtils::getSidebarStyle()
{
    return QString(
        "QWidget {"
        "  background-color: %1;"
        "  border-right: 1px solid %2;"
        "}"
    ).arg(Colors::SURFACE(), Colors::BORDER());
}

QString StyleUtils::getContentAreaStyle()
{
    return QString(
        "QWidget {"
        "  background-color: %1;"
        "}"
    ).arg(Colors::BACKGROUND()) + getScrollAreaStyle();
}

QString StyleUtils::getScrollAreaStyle()
{
    return QString(
        "QScrollArea {"
        "  border: none;"
        "  background: transparent;"
        "}"
        "QScrollBar:vertical {"
        "  background-color: %1;"
        "  width: 12px;"
        "  border-radius: 6px;"
        "  margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background-color: %2;"
        "  border-radius: 6px;"
        "  min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "  background-color: %3;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  border: none;"
        "  background: none;"
        "}"
    ).arg(Colors::SURFACE(), Colors::BORDER(), Colors::SECONDARY());
}

QString StyleUtils::getCardStyle()
{
    return QString(
        "QWidget {"
        "  background-color: %1;"
        "  border: 1px solid %2;"
        "  border-radius: %3px;"
        "  padding: %4px;"
        "  margin: %5px;"
        "}"
        "QWidget:hover {"
        "  border-color: %6;"
        "  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);"
        "}"
    ).arg(Colors::BACKGROUND(), Colors::BORDER())
     .arg(Dimensions::BORDER_RADIUS_LARGE)
     .arg(Dimensions::PADDING_LARGE)
     .arg(Dimensions::PADDING_SMALL)
     .arg(Colors::SECONDARY());
}

QString StyleUtils::getMediaCardStyle()
{
    return QString(
        "QWidget {"
        "  border: 1px solid %1;"
        "  border-radius: %2px;"
        "  margin: %3px;"
        "  background: %4;"
        "}"
    ).arg(Colors::BORDER())
     .arg(Dimensions::MARGIN_MEDIUM)
     .arg(Dimensions::MARGIN_MEDIUM)
     .arg(Colors::BACKGROUND());
}

// Sezione Etichette
QString StyleUtils::getTitleLabelStyle()
{
    return QString(
        "QLabel {"
        "  color: %1;"
        "  font-size: %2px;"
        "  font-weight: %3;"
        "  margin-bottom: %4px;"
        "}"
    ).arg(Colors::TEXT())
     .arg(FontSizes::XLARGE)
     .arg(FontWeights::BOLD)
     .arg(Dimensions::PADDING_SMALL);
}

QString StyleUtils::getSubtitleLabelStyle()
{
    return QString(
        "QLabel {"
        "  color: %1;"
        "  font-size: %2px;"
        "  font-weight: %3;"
        "  margin-bottom: %4px;"
        "}"
    ).arg(Colors::TEXT())
     .arg(FontSizes::NORMAL)
     .arg(FontWeights::SEMIBOLD)
     .arg(Dimensions::SPACING_SMALL);
}

QString StyleUtils::getBodyLabelStyle()
{
    return QString(
        "QLabel {"
        "  color: %1;"
        "  font-size: %2px;"
        "  font-weight: %3;"
        "  line-height: 1.5;"
        "}"
    ).arg(Colors::TEXT_SECONDARY())
     .arg(FontSizes::NORMAL)
     .arg(FontWeights::NORMAL);
}

QString StyleUtils::getCategoryButtonStyle()
{
    return QString(
        "QPushButton {"
        "  text-align: left;"
        "  padding: %1px %2px;"
        "  border: none;"
        "  border-radius: %3px;"
        "  margin: %4px 0px;"
        "  font-size: %5px;"
        "  font-weight: %6;"
        "  color: %7;"
        "  background-color: transparent;"
        "}"
        "QPushButton:hover {"
        "  background-color: %8;"
        "  color: %9;"
        "}"
        "QPushButton:checked {"
        "  background-color: %10;"
        "  color: white;"
        "  font-weight: %11;"
        "}"
    ).arg(Dimensions::PADDING_MEDIUM)
     .arg(Dimensions::PADDING_LARGE)
     .arg(Dimensions::BORDER_RADIUS)
     .arg(Dimensions::MARGIN_SMALL)
     .arg(FontSizes::NORMAL)
     .arg(FontWeights::MEDIUM)
     .arg(Colors::TEXT_SECONDARY())
     .arg(Colors::SURFACE())
     .arg(Colors::TEXT())
     .arg(Colors::PRIMARY())
     .arg(FontWeights::SEMIBOLD);
}