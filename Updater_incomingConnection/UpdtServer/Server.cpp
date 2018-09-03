#include "Server.h"
#include "Socket.h"
#include "DomDocument.h"
#include "SocketHandle.h"
#include <QThread>

Server::Server(const QString &fileName, QObject *parent):
    fileName(fileName), QTcpServer(parent)
{
    //获取服务器版本信息
    QString updater("update.xml");
    DomDocument dom(updater);
    dom.readXml(version);
    qDebug() << "版本：" << version << fileName;
    QFile upfile1(fileName);
    QFile upfile2(updater);
    if (upfile1.open(QIODevice::ReadOnly) && upfile2.open(QIODevice::ReadOnly)) {
        file1 = upfile1.readAll();
        qDebug() << "文件大小：" << file1.size();
        file2 = upfile2.readAll();
        upfile1.close();
        upfile2.close();
    }
}
Server::~Server()
{

}
void Server::incomingConnection(qintptr socketDescriptor)
{
    Socket *clientSocket = new Socket(socketDescriptor, version, file1, file2);
    QThread * thread = new QThread(clientSocket);

    connect(clientSocket,SIGNAL(readyRead()),clientSocket,SLOT(readDataSlot()));
    //connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    //下面这句会报错：QThread::wait: Thread tried to wait on itself
//    connect(clientSocket,&Socket::disconnected,thread,&QThread::quit);//断开连接时线程退出
//    connect(thread, SIGNAL(finished()),clientSocket, SLOT(deleteLater()));
    clientSocket->moveToThread(thread);//把tcp类移动到新的线程
    thread->start();
    SocketHandle *instance = SocketHandle::getInstance();
    instance->insertClient(clientSocket);
    qDebug() << __FUNCTION__ << QThread::currentThreadId();
}
