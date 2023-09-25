#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    mainWindow.setWindowTitle("TFTP Client");
    return QApplication::exec();
}
