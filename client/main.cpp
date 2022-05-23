#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationDisplayName(MainWindow::tr("client"));
    MainWindow main;
    main.show();
    return a.exec();
}
