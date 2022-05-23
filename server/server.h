#ifndef SERVER_H
#define SERVER_H

#include <QDialog>
#include <QString>
#include <QList>

QT_BEGIN_NAMESPACE
class QLabel;
class QTcpServer;
class QTcpSocket;
QT_END_NAMESPACE

class server : public QDialog
{
    Q_OBJECT
public:
    explicit server(QWidget * parent = nullptr);
    QTcpSocket *socket;
private:
    void initServer();
    QLabel *status = nullptr;
    QVector <QTcpSocket*> sockets;
    QTcpServer *tcpServer = nullptr;
    void send(QString str);
    QByteArray Data;
private slots:
    void newConnection(quintptr socketDescriptor);
    void onReadyRead();
};

#endif // SERVER_H
