#ifndef SOCKET_H
#define SOCKET_H
#include <QDebug>
#include <QTcpSocket>
#include <QDataStream>
#include <QDate>
#include <QByteArray>
#include <QFile>
#include <QThread>

class Socket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit Socket(qintptr socketId, const QString &version="", QByteArray file1 = 0, \
                    QByteArray file2 = 0, QObject * parent = 0);
    ~Socket() {}

signals:

private slots:
    void readDataSlot();

public slots:

private:
    qintptr     socketId;
    QString     version;
    QByteArray  file1;
    QByteArray  file2;
    quint64     nextBlockSize;
};

#endif // Socket_H
