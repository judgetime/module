#include "Server.h"
#include "Socket.h"
#include "DomDocument.h"
#include <QThread>

Server::Server(QObject *parent):QTcpServer(parent)
{
    clients = new QList<Socket *>;
    //本地读取要传输的文件
    QFile file("version.dat");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        version = stream.readLine();
        file.close();
    }
    QFile upfile1("MRP.exe");
    QFile upfile2("common.dll");
    if (upfile1.open(QIODevice::ReadOnly) && upfile2.open(QIODevice::ReadOnly)) {
        file1 = upfile1.readAll();
        file2 = upfile2.readAll();
    }

    nextBlockSize = 0;
    //获取服务器版本信息
    QString fileName("update.xml");
    DomDocument dom(fileName);
    dom.writeXml();
    dom.updateXml("1.0");
    dom.readXml(version);
    qDebug() << "版本：" << version;
}
Server::~Server()
{
    delete clients;
}
void Server::incomingConnection(qintptr socketDescriptor)
{
    Socket *clientSocket = new Socket(socketDescriptor, version, file1, file2);
    QThread * thread = new QThread(clientSocket);

    //connect(clientSocket, SIGNAL(readyRead()), this, SLOT(readClient()));//当有数据来时，触发读取客户端的信息
    //connect(clientSocket,SIGNAL(disconnected()),clientSocket,SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    //connect(clientSocket,&Socket::disconnected,thread,&QThread::quit);//断开连接时线程退出
    connect(clientSocket,SIGNAL(readyRead()),clientSocket,SLOT(readDataSlot()));
    connect(clientSocket, SIGNAL(disconnected()),clientSocket, SLOT(deleteLater()));
    clientSocket->moveToThread(thread);//把tcp类移动到新的线程
    thread->start();
    qDebug() << __FUNCTION__ << QThread::currentThreadId();

    clients->append(clientSocket);
}
