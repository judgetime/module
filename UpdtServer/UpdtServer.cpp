#include "UpdtServer.h"
#include "ui_UpdtServer.h"
#include "DomDocument.h"
#include "Thread.h"

UpdtServer::UpdtServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UpdtServer)
{
    ui->setupUi(this);
    //将需要更新的文件读入到file1和file2中
    QFile file("version.dat");
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        version = stream.readLine();
        file.close();
    }
    QFile upfile1("MRP.exe");
    QFile upfile2("common.dll");
    if (upfile1.open(QIODevice::ReadOnly)&&upfile2.open(QIODevice::ReadOnly))
    {
        file1 = upfile1.readAll();
        file2 = upfile2.readAll();
    }


    nextBlockSize = 0;
    tcpServer = new QTcpServer(this);
    if(!tcpServer->listen(QHostAddress::Any,8080))
    //if(!tcpServer->listen(QHostAddress("192.168.1.100"),8080))
    {  //监听本地主机的8080端口，如果出错就输出错误信息，并关闭
        qDebug() << tcpServer->errorString();
        close();
    }
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(incomingConnection()));
}

UpdtServer::~UpdtServer()
{
    delete ui;
}

void UpdtServer::incomingConnection()
{
    Thread * thread = new ConnectThread(this,socketId,version,file1,file2);

    TcpReciever = tcpServer->nextPendingConnection();//套接字器指向该链接
    connect(TcpReciever, SIGNAL(readyRead()), this, SLOT(readClient()));//当有数据来时，触发读取客户端的信息
    QString ipCli =TcpReciever->peerAddress().toString();//链接地址
    qint16 portCli = TcpReciever->peerPort();//链接方端口
    QString temp = QString("客户端:[%1:%2]:连接成功").arg(ipCli).arg(portCli);
    ipCli=TcpReciever->localAddress().toString();
    portCli=TcpReciever->localPort();
    temp += QString("服务器:[%1:%2]").arg(ipCli).arg(portCli);
    ui->m_Label->setText(temp);//显示服务器客户端分别地址和ip

    //获取服务器版本信息
    QString fileName("update.xml");
    DomDocument dom(fileName);
    dom.writeXml();
    dom.updateXml("1.0");
    dom.readXml(version);
    qDebug() << "版本：" << version;

    //发送当前服务器上的客户端版本信息给升级客户端
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
    out.setVersion(QDataStream::Qt_4_6);//设置数据流的版本，客户端和服务器端使用的版本要相同
    out<<quint64(0)<<quint16(0x0001)<<version;//要发送的数据放到out
    out.device()->seek(0);
    out<<(quint16)(block.size()-sizeof(quint16));//要发送的数据放到out
    TcpReciever->write(block);
    ui->statusLabel->setText("send message successful!!!");//发送数据成功后，显示提示

    connect(TcpReciever,SIGNAL(disconnected()),TcpReciever,SLOT(deleteLater()));
    //TcpReciever->disconnectFromHost();
}

void UpdtServer::readClient()
{
    //将读取的客户端信息显出来
//    TcpReciever->waitForReadyRead();
//    QString str = TcpReciever->readAll();
//    ui->textEdit->append(str);
    //创建数据流
    QDataStream in(TcpReciever);
    in.setVersion(QDataStream::Qt_4_6);
    while(1) {
        if (nextBlockSize == 0) {
            if (TcpReciever->bytesAvailable() < sizeof(quint64)) {
                break;
            }
            in >> nextBlockSize;
        }
        if ((quint64)(TcpReciever->bytesAvailable()) < nextBlockSize) {
            break;
        }
        quint16 protocol;
        in >> protocol;
        qDebug() << "CR_MRP_Update:protocol:" << QString::number(protocol,16);
        if(protocol == 1) {
            //升级
            QByteArray fblock;
            QDataStream out(&fblock, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_5);
            out << quint64(0) << quint16(0x0002) << file1 << file2;
            out.device()->seek(0);
            out << quint64(fblock.size() - sizeof(quint64));
            TcpReciever->write(fblock);
            qDebug() << "CR_MRP_Update:size:" << QString::number(fblock.size() - sizeof(quint64));
        }
        nextBlockSize = 0;
    }
}
