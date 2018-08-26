#include "Server.h"
#include "Socket.h"
#include "DomDocument.h"
#include <QThread>

Server::Server(QObject *parent):QTcpServer(parent)
{

}

Server::Server(QByteArray file1, QByteArray file2, QObject *parent):
    file1(file1), file2(file2), QTcpServer(parent)
{
    nextBlockSize = 0;
    //获取服务器版本信息
    QString fileName("update.xml");
    DomDocument dom(fileName);
//    dom.writeXml();
//    dom.updateXml("1.0");
    dom.readXml(version);
    qDebug() << "版本：" << version;
}

Server::~Server()
{
    this->close();
    QList<Socket*>::iterator it = clients.begin();
    for (; it != clients.end(); )
    {
        Socket* sock = *it;
        clients.erase(it++);
        delete sock;
        sock = NULL;
    }
    clients.clear();
}
void Server::incomingConnection(qintptr socketDescriptor)
{
    Socket *clientSocket = new Socket(socketDescriptor, version, file1, file2);
    connect(clientSocket,SIGNAL(readyRead()),clientSocket,SLOT(readDataSlot()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(socketDisconnect()));

    QThread * thread = new QThread(clientSocket);
    connect(clientSocket, SIGNAL(disconnected()), thread, SLOT(quit()));//断开连接时线程退出
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    //connect(clientSocket, SIGNAL(disconnected()),clientSocket, SLOT(deleteLater()));
    clientSocket->moveToThread(thread);//把tcp类移动到新的线程
    thread->start();
    emit newConnection();   //文档要求继承本函数需要发射此信号，此处没有与此信号连接的槽
    qDebug() << __FUNCTION__ << QThread::currentThreadId();

    clients.append(clientSocket);
}

void Server::socketDisconnect()
{
    Socket* socket = (Socket*)sender();
    QList<Socket*>::iterator it = find(clients.begin(), clients.end(), socket);
    if (it != clients.end())
    {
        clients.erase(it);
        delete socket;
        socket = NULL;
    }
}
