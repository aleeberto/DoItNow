#include <QApplication>
#include "src/ui/mainWindow.h"
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}