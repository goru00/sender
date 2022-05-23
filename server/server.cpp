#include "server.h"

#include <QtWidgets>
#include <QtNetwork>
#include <QtCore>

server::server(QWidget *parent)
    : QDialog(parent)
    , status(new QLabel)
{
    status->setTextInteractionFlags(Qt::TextBrowserInteraction);

    initServer();

    auto quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    QVBoxLayout * mainLayout = nullptr;
    if (QGuiApplication::styleHints()->showIsFullScreen() || QGuiApplication::styleHints()->showIsMaximized()) {
        auto outerVerticalLayout = new QVBoxLayout(this);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
        auto outerHorizontalLayout = new QHBoxLayout;
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        auto groupBox = new QGroupBox(QGuiApplication::applicationDisplayName());
        mainLayout = new QVBoxLayout(groupBox);
        outerHorizontalLayout->addWidget(groupBox);
        outerHorizontalLayout->addItem(outerHorizontalLayout);
        outerVerticalLayout->addLayout(outerHorizontalLayout);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
    } else {
        mainLayout = new QVBoxLayout(this);
    }
    mainLayout->addWidget(status);
    mainLayout->addLayout(buttonLayout);
    setWindowTitle(QGuiApplication::applicationDisplayName());
}


void server::initServer() {
    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen()) {
        QMessageBox::critical(this, tr("start server"), tr("unable server").arg(tcpServer->errorString()));
        close();
        return;
    }
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int index = 0; index < ipAddressesList.size(); index++) {
        if (ipAddressesList.at(index) != QHostAddress::LocalHost &&
                ipAddressesList.at(index).toIPv4Address()) {
            ipAddress = ipAddressesList.at(index).toString();
            break;
        }
    }
    if (ipAddress.isEmpty()) ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    status->setText(tr("the server is running on\n\nIP: %1\nПорт: %2\n\n").arg(ipAddress).arg(tcpServer->serverPort()));

}

void server::newConnection(quintptr socketDescriptor) {
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &server::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    sockets.push_back(socket);

}

void server::onReadyRead() {
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);
    if (in.status() == QDataStream::Ok) {
        qDebug() << "read...";
        QString str;
        in >> str;
        qDebug() << str;
    } else {
        qDebug() << "error";
    }
}

void server::send(QString str) {
    Data.clear();
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << str;
    for (int index = 0; index < sockets.size(); index++) {
        sockets[index]->write(Data);
    }
}
