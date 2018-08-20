#include "Thread.h"

Thread::Thread(QObject * parent,int socketId,const QString &version,QByteArray file1,QByteArray file2) :
    QObject(parent)
{
    nextBlockSize = 0;
    clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketId);
    this->version = version;
    this->file1 = file1;
    this->file2 = file2;
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(threadOver()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(communicate()));
    qDebug()<<"CR_MRP_Update:connect to server successful,client id is"<<socketId;
    //发送当前服务器上的客户端版本信息给升级客户端
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint64(0) << quint16(0x0001) << version;
    out.device()->seek(0);
    out << quint64(block.size() - sizeof(quint16));
    clientSocket->write(block);
}
void Thread::communicate()
{
    //创建数据流
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_4_6);
    while(1)
    {
        if (nextBlockSize == 0)
        {
            if (clientSocket->bytesAvailable() < sizeof(quint64))
            {
                break;
            }
            in >> nextBlockSize;
        }
        if ((quint64)(clientSocket->bytesAvailable())<nextBlockSize)
        {
            break;;
        }
        quint16 protocol;
        in>>protocol;
        qDebug()<<"CR_MRP_Update:protocol:"<<QString::number(protocol,16);
        if(protocol==1)
        {
            //升级
            QByteArray fblock;
            QDataStream fout(&fblock, QIODevice::WriteOnly);
            fout.setVersion(QDataStream::Qt_4_5);
            fout << quint64(0) << quint16(0x0002) << file1 << file2;
            fout.device()->seek(0);
            fout << quint64(fblock.size() - sizeof(quint64));
            clientSocket->write(fblock);
            qDebug() << "CR_MRP_Update:size:" << QString::number(fblock.size() - sizeof(quint64));
        }
        nextBlockSize = 0;
    }
}

void Thread::threadOver()
{
    qDebug()<<"CR_MRP_Update:a client connection closed.../n";
    clientSocket->deleteLater();
    this->deleteLater();
}
