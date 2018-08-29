#include "SocketManager.h"
#include "Sockethandle.h"
#include <QFile>

QMutex SocketManager::mutex;
SocketManager* SocketManager::instance = 0;

SocketManager* SocketManager::getInstance ()
{
    if(0 == instance){
        mutex.lock ();
        if(0 == instance)
            instance = new SocketManager();
        mutex.unlock ();
    }
    return instance;
}
SocketManager::SocketManager()
{
}
void SocketManager::insertClient (Sockethandle *sh)
{
    clients.push_back (sh);
}
