#ifndef UPDTSERVER_H
#define UPDTSERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class UpdtServer;
}

class UpdtServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit UpdtServer(QWidget *parent = 0);
    ~UpdtServer();

private slots:
    void acceptConnection();
    void readClient();

private:
    Ui::UpdtServer *ui;
    QTcpServer *tcpServer;
    QTcpSocket *TcpReciever;
};

#endif // UPDTSERVER_H
