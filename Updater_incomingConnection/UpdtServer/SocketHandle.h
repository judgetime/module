#ifndef SOCKETHANDLE_H
#define SOCKETHANDLE_H
#include <QVector>
#include <QMutex>

class Socket;

class SocketHandle
{
public:
    static SocketHandle* getInstance();
    void insertClient(Socket* sh);

private:
    SocketHandle();

    static QMutex mutex;
    static SocketHandle* instance;
    QVector<Socket *> clients;
};

#endif // SOCKETHANDLE_H
