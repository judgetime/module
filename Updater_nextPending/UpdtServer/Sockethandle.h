#ifndef SOCKETHANDLE_H
#define SOCKETHANDLE_H
#include <QDebug>
#include <QTcpSocket>
#include <QDataStream>
#include <QDate>
#include <QByteArray>
#include <QFile>
#include <QThread>
#include <QObject>

class Sockethandle : public QObject
{
    Q_OBJECT
public:
    explicit Sockethandle(QTcpSocket* socket, const QString &version="", QByteArray file1 = 0, \
                    QByteArray file2 = 0, QObject * parent = 0);
    ~Sockethandle() {}

signals:
    void sendData(QTcpSocket*, QByteArray);

private slots:
    void readyReadSlot();

public slots:

private:
    QTcpSocket* clientSocket;
    QString     version;
    QByteArray  file1;
    QByteArray  file2;
};

#endif // Sockethandle_H
