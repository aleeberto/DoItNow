#ifndef TOPMENU_HEADER
#define TOPMENU_HEADER

#include <QWidget>
#include <QPushButton>

class TopMenuWidget : public QWidget {
    Q_OBJECT
public:
    TopMenuWidget(QWidget* parent = nullptr);
    void updateThemeButtonIcon();
    
signals:
    void uploadRequested();    // Carica un file JSON esistente
    void exportRequested();    // Crea un nuovo file JSON e eventualmente esporta
    void closeRequested();     // Chiude il JSON corrente
    void createRequested();    // Crea un nuovo media
    void themeToggleRequested(); // Toggle del tema

private:
    QPushButton* themeToggleButton;
};

#endif // TOPMENU_HEADER