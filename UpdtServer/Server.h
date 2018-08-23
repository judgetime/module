#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QByteArray>
#include <QList>

class Socket;

class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = 0);
    ~Server();

private slots:
    virtual void incomingConnection(qintptr socketDescriptor);

private:
    quint64         nextBlockSize;
    QString         version;
    QByteArray      file1;
    QByteArray      file2;
    QList<Socket *> *clients;
};

#endif // SERVER_H
