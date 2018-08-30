#include "Socket.h"
#include <QThread>

Socket::Socket(qintptr socketId,const QString &version,QByteArray file1,QByteArray file2, QObject * parent) :
    socketId(socketId), version(version), file1(file1), file2(file2), QTcpSocket(parent)
{
    this->setSocketDescriptor(socketId);

    nextBlockSize = 0;    
//    this->version = version;
//    this->file1 = file1;
//    this->file2 = file2;
}

void Socket::readDataSlot()
{
    nextBlockSize = 0;
    qDebug() << "Socket::sentData"<< __FUNCTION__ << QThread::currentThreadId();
    //创建数据流
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_5_8);
    if (nextBlockSize == 0) {
        if (this->bytesAvailable() < sizeof(quint64)){
            return;
        }
        in >> nextBlockSize;
    }
    if ((quint64)(this->bytesAvailable())<nextBlockSize) {
        return;
    }
    quint16 protocol;
    in>>protocol;
    qDebug()<<"CR_MRP_Update:protocol:"<<QString::number(protocol,16);
    switch (protocol) {
    case 0x0001:{
        QString str;
        in >> str;
        qDebug()<<str;
        //发送当前服务器上的客户端版本信息给升级客户端
        QByteArray block; //用于暂存我们要发送的数据
        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
        out.setVersion(QDataStream::Qt_5_8);//设置数据流的版本，客户端和服务器端使用的版本要相同
        out<<quint64(0)<<quint16(0x0001)<<version;//要发送的数据放到out
        out.device()->seek(0);
        out<<(quint64)(block.size()-sizeof(quint64));//要发送的数据放到out
        this->write(block);
        qDebug()<<"向客户端发送版本号";
        break;
    }
    case 0x0002:{
        QString str;
        in >> str;
        qDebug()<<str;
        //升级
        QByteArray fblock;
        QDataStream fout(&fblock, QIODevice::WriteOnly);
        fout.setVersion(QDataStream::Qt_4_5);
        fout << quint64(0) << quint16(0x0002) << file1 << file2;
        fout.device()->seek(0);
        fout << quint64(fblock.size() - sizeof(quint64));
        this->write(fblock);
        qDebug() << "CR_MRP_Update:size:" << QString::number(fblock.size() - sizeof(quint64));
        break;
    }
    default:
        break;
    }
}

