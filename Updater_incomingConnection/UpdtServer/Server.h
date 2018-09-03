#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QByteArray>

class Socket;

class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(const QString &fileName, QObject *parent = 0);
    ~Server();

private slots:
    virtual void incomingConnection(qintptr socketDescriptor);

private:
    QString     fileName;   //选择的需要更新的文件路径
    QString     version;    //版本
    QByteArray  file1;      //读取的程序二进制内容
    QByteArray  file2;      //读取的update.xml二进制内容
};

#endif // SERVER_H
