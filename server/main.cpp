#include <QApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationDisplayName(server::tr("server"));
    server serv;
    serv.show();
    return a.exec();
}
