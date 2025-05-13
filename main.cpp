#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application info
    QApplication::setApplicationName("Python Compiler");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("Your Organization");

    // Create and show the main window
    MainWindow mainWindow;
    mainWindow.show();

    // Enter the event loop
    return app.exec();
}
