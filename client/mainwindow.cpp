#include "mainwindow.h"
#include <QtWidgets>
#include <QtNetwork>

MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent)
    , host(new QComboBox)
    , port(new QLineEdit)
    , connectBtn(new QPushButton(tr("Подключиться")))
    , sendBtn(new QPushButton(tr("Отправить сообщение")))
    , browser(new QTextBrowser)
    , input(new QLineEdit)
    , socket(new QTcpSocket(this))
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    host->setEditable(true);
    QString name = QHostInfo::localHostName();
    if (!name.isEmpty()) {
        host->addItem(name);
        QString domain = QHostInfo::localDomainName();
        if (!domain.isEmpty()) host->addItem(name + QChar('.') + domain);
    }
    if (name != QLatin1String("localhost")) host->addItem("localhost");
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int index = 0; index < ipAddressesList.size(); index++) {
        if (!ipAddressesList.at(index).isLoopback()) host->addItem(ipAddressesList.at(index).toString());
    }
    for (int index = 0; index < ipAddressesList.size(); index++) {
        if (ipAddressesList.at(index).isLoopback()) host->addItem(ipAddressesList.at(index).toString());
    }

    port->setValidator(new QIntValidator(1, 65535, this));
    auto hostLabel = new QLabel(tr("&Адрес: "));
    hostLabel->setBuddy(host);
    auto portLabel = new QLabel(tr("&Порт: "));
    portLabel->setBuddy(port);
    auto browserLabel = new QLabel(tr("&Вывод"));
    browserLabel->setBuddy(browser);
    auto inputLabel = new QLabel(tr("&Ввод"));
    inputLabel->setBuddy(input);

    sendBtn->setDefault(true);
    sendBtn->setEnabled(false);
    connectBtn->setDefault(true);
    connectBtn->setEnabled(false);

    status = new QLabel(tr("status"));

    auto quitButton = new QPushButton(tr("Выйти"));
    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(connectBtn, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);
    buttonBox->addButton(sendBtn, QDialogButtonBox::ActionRole);

    auto textBrowser = new QTextBrowser(this);


    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_6_2);

    connect(host, &QComboBox::editTextChanged, this, &MainWindow::enableButton);
    connect(port, &QLineEdit::textChanged, this, &MainWindow::enableButton);
    connect(connectBtn, &QAbstractButton::clicked, this, &MainWindow::newRequest);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);
    connect(socket, &QIODevice::readyRead, this, &MainWindow::onReadyRead);
    connect(sendBtn, &QAbstractButton::clicked, this, &MainWindow::newRequest);
    connect(socket, &QAbstractSocket::errorOccurred, this, &MainWindow::showError);

    QGridLayout *mainLayout = nullptr;
    if (QGuiApplication::styleHints()->showIsFullScreen() ||
            QGuiApplication::styleHints()->showIsMaximized()) {
        auto outerVerticalLayout = new QVBoxLayout(this);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
        auto outerHorizontalLayout = new QHBoxLayout;
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        auto groupBox = new QGroupBox(QGuiApplication::applicationDisplayName());
        mainLayout = new QGridLayout(groupBox);
        outerHorizontalLayout->addWidget(groupBox);
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        outerVerticalLayout->addLayout(outerHorizontalLayout);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
    } else {
        mainLayout = new QGridLayout(this);
    }

    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(host, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(port, 1, 1);
    mainLayout->addWidget(status, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
    mainLayout->addWidget(browserLabel, 4, 0);
    mainLayout->addWidget(textBrowser, 5, 0, 1, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    port->setFocus();
}

void MainWindow::newRequest() {
    connectBtn->setEnabled(false);
    sendBtn->setEnabled(false);
    socket->abort();
    socket->connectToHost(host->currentText(), port->text().toInt());
}

void MainWindow::onReadyRead() {
    in.startTransaction();
    QString next;
    in >> next;
    if (!in.commitTransaction()) return;
    if (next == curr) {
        QTimer::singleShot(0, this, &MainWindow::newRequest);
        return;
    }
    curr = next;
    status->setText(curr);
    connectBtn->setEnabled(true);
    sendBtn->setEnabled(true);
}

void MainWindow::showError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(socket->errorString()));
    }
    connectBtn->setEnabled(true);
}

void MainWindow::enableButton() {
    connectBtn->setEnabled(!host->currentText().isEmpty() && !port->text().isEmpty());
}
