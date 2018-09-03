#include "Socket.h"
#include <QThread>

Socket::Socket(qintptr socketId,const QString &version,QByteArray file1,QByteArray file2, QObject * parent) :
    socketId(socketId), version(version), file1(file1), file2(file2), QTcpSocket(parent)
{
    this->setSocketDescriptor(socketId);
    nextBlockSize = 0;
}
/*
 * 通信协议:
 *      服务器接收：
 *              0x0001表示客户端请求服务器返回一个版本号
 *              0x0002表示服务器将最新程序的xml文件和二进制文件返回给客户端
 * QDataStream的格式为：数据长度（quint64）,通信协议（quint16）,具体内容
 */
void Socket::readDataSlot()
{
    qDebug() << __FUNCTION__ << QThread::currentThreadId();
    //while( this->bytesAvailable() > 0 ) {
        QDataStream in(this);//创建数据流
        in.setVersion(QDataStream::Qt_5_8);
        if (nextBlockSize == 0) {
            if (this->bytesAvailable() < sizeof(quint64)){
//                break;
                return;
            }
            in >> nextBlockSize;
        }
        if ((quint64)(this->bytesAvailable()) < nextBlockSize) {
//            break;
            return;
        }
        quint16 protocol;
        in>>protocol;
        switch (protocol) {
        case 0x0001:{
            //发送当前服务器上的客户端版本信息给升级客户端
            QByteArray block; //用于暂存我们要发送的数据
            QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
            out.setVersion(QDataStream::Qt_5_8);//设置数据流的版本，客户端和服务器端使用的版本要相同
            out << quint64(0) << quint16(0x0001) << version;//要发送的数据放到out
            out.device()->seek(0);
            out << (quint64)(block.size() - sizeof(quint64));//要发送的数据放到out
            this->write(block);
            qDebug() << "向客户端发送版本号";
            break;
        }
        case 0x0002:{
            QByteArray fblock;
            QDataStream fout(&fblock, QIODevice::WriteOnly);
            fout.setVersion(QDataStream::Qt_5_8);
            fout << quint64(0) << quint16(0x0002) << version << file1 << file2;
            fout.device()->seek(0);
            fout << quint64(fblock.size() - sizeof(quint64));
            this->write(fblock);
            qDebug() << "向客户端发送文件,size:" << fblock.size() - sizeof(quint64);
            break;
        }
        default:
            break;
        }
        nextBlockSize = 0;
    //}
}

