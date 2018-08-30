#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#include <QVector>
#include <QMutex>

class Sockethandle;

class SocketManager
{
public:
    static SocketManager* getInstance();
    void insertClient(Sockethandle* sh);
    //QVector<Sockethandle*> getAllClient() const;

private:
    explicit SocketManager();

    static QMutex mutex;
    static SocketManager* instance;
    QVector<Sockethandle *> clients;
};

#endif // SOCKETMANAGER_H
