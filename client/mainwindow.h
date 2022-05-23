#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDataStream>
#include <QDialog>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QLineEdit;
class QTextBrowser;
class QPushButton;
class QTcpSocket;
QT_END_NAMESPACE

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void newRequest();
    void onReadyRead();
    void showError(QAbstractSocket::SocketError socketError);
    void enableButton();
private:
    QComboBox *host = nullptr;
    QLineEdit *port = nullptr;
    QLabel *status = nullptr;
    QPushButton *connectBtn = nullptr;
    QPushButton * sendBtn = nullptr;
    QTextBrowser *browser = nullptr;
    QLineEdit *input = nullptr;
    QTcpSocket *socket = nullptr;
    QDataStream in;
    QString curr;
};
#endif // MAINWINDOW_H
