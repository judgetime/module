#include "SocketHandle.h"
#include "Socket.h"

QMutex SocketHandle::mutex;
SocketHandle* SocketHandle::instance = 0;
SocketHandle::SocketHandle()
{

}
SocketHandle* SocketHandle::getInstance ()
{
    if(0 == instance){
        mutex.lock ();
        if(0 == instance)
            instance = new SocketHandle();
        mutex.unlock ();
    }
    return instance;
}
void SocketHandle::insertClient (Socket *sh)
{
    clients.push_back (sh);
}
