#ifndef THREAD_H
#define THREAD_H
#include <QDebug>
#include <QTcpSocket>
#include <QDataStream>
#include <QDate>
#include <QByteArray>
#include <QFile>
#include <QThread>

class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(QObject *parent = nullptr);
    ~Thread() {}
    ConnectThread(QObject * parent = 0 , int socketId = 0, const \
                  QString &version="", QByteArray file1 = 0, QByteArray file2 = 0);
signals:

private slots:
    void threadOver();
    void communicate();
private:
    int socketId;
    QTcpSocket  *clientSocket;
    quint64     nextBlockSize;
    QString     version;
    QByteArray  file1;
    QByteArray  file2;
};

#endif // THREAD_H
