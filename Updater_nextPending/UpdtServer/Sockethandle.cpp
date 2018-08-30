#include "Sockethandle.h"
#include <QThread>

Sockethandle::Sockethandle(QTcpSocket* socket, const QString &version, QByteArray file1 , \
                           QByteArray file2, QObject * parent) :
    clientSocket(socket), version(version), file1(file1), file2(file2), QObject(parent)
{
    connect (clientSocket, SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
}

void Sockethandle::readyReadSlot()
{
    qDebug() << "线程"<< __FUNCTION__ << QThread::currentThreadId();
    if( clientSocket->bytesAvailable() <= 0 )
        return;
    quint64 nextBlockSize = 0;
    //创建数据流
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_4_0);
    //while(1) {
        //如果是刚开始接收数据
        if (nextBlockSize == 0) {
            //判断接收的数据是否有两字节，也就是文件的大小信息
            //如果有则保存到blockSize变量中，没有则返回，继续接收数据
            if ((quint64)clientSocket->bytesAvailable() < sizeof(quint64)) {
                qDebug() << "没有文件大小。";
                return;
            }
            in >> nextBlockSize;
            qDebug() << "接受的数据大小：" << nextBlockSize;
        }
        if(nextBlockSize==0xFFFF)
            return;
        //如果没有得到全部的数据，则返回，继续接收数据
        if ((quint64)(clientSocket->bytesAvailable()) < nextBlockSize) {
            qDebug() << "没有得到全部的数据。";
            return;
        }
        /*通信协议:
         *      客户端接收：
         *              0x0001表示从客户端发送过来版本号请求，请求服务器发送最新版本号给客户端
         *              0x0002表示服务器将最新程序的xml文件和二进制文件发送个客户端
         * QDataStream的格式为：数据长度（quint64）,通信协议（quint16）,具体内容
         */
        quint16 protocol;
        in >> protocol;
        qDebug() << "server:protocol:" << QString::number(protocol,16);
        switch (protocol) {
        case 0x0001:{
            QString str;
            in >> str;
            //发送当前服务器上的客户端版本信息给升级客户端
            QByteArray block = 0; //用于暂存我们要发送的数据
            QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
            out.setVersion(QDataStream::Qt_4_0);//设置数据流的版本，客户端和服务器端使用的版本要相同
            out << quint64(0) << quint16(0x0001) << version;//要发送的数据放到out
            out.device()->seek(0);
            out << (quint64)(block.size() - sizeof(quint64));//数据大小
            emit sendData(clientSocket, block);
            qDebug() << "第一次收到客户端数据并发送版本。";
            break;
        }
        case 0x0002:{
            //升级
            QByteArray fblock = 0;
            QDataStream fout(&fblock, QIODevice::WriteOnly);
            fout.setVersion(QDataStream::Qt_4_0);
            fout << quint64(0) << quint16(0x0002) << file1 << file2;
            fout.device()->seek(0);
            fout << quint64(fblock.size() - sizeof(quint64));
            emit sendData(clientSocket, fblock);
             qDebug() << "第二次收到客户端数据，发送文件。";
            break;
        }
        default:
            break;
        }
    //}
}
