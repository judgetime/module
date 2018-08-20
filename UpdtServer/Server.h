#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QByteArray>

class Server : public QServer
{
    Q_OBJECT

public:
    Server(QObject *parent = 0);
    ~Server() {}
private slots:
    void incomingConnection(int socketId);
private:
    QString     version;        //服务器上的最新版本
    QByteArray  file1;       //保存客户端要更新的文件
    QByteArray  file2;
};

#endif // SERVER_H
