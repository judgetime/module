#include "UpdtServer.h"
#include "ui_UpdtServer.h"
#include "DomDocument.h"
UpdtServer::UpdtServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UpdtServer)
{
    ui->setupUi(this);
    tcpServer = new QTcpServer(this);
    if(!tcpServer->listen(QHostAddress::Any,8080))
    //if(!tcpServer->listen(QHostAddress("192.168.1.100"),6666))
    {  //监听本地主机的6666端口，如果出错就输出错误信息，并关闭
        qDebug() << tcpServer->errorString();
        close();
    }
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
//    connect(this,SIGNAL(readyRead()),this, SLOT(ReadAndParseData()), Qt::DirectConnection);

    QString fileName("update.xml");
    DomDocument dom(fileName);
    dom.writeXml();
    dom.updateXml("1.0");
    QString version;
    dom.readXml(version);
    qDebug() << "版本：" << version;
}

UpdtServer::~UpdtServer()
{
    delete ui;
}

void UpdtServer::acceptConnection()
{
    TcpReciever = tcpServer->nextPendingConnection();//套接字器指向该链接
    connect(TcpReciever, SIGNAL(readyRead()), this, SLOT(readClient()));//当有数据来时，触发读取客户端的信息
    QString ipCli =TcpReciever->peerAddress().toString();//链接地址
    qint16 portCli = TcpReciever->peerPort();//链接方端口
    QString temp = QString("客户端:[%1:%2]:连接成功").arg(ipCli).arg(portCli);
    ipCli=TcpReciever->localAddress().toString();
    portCli=TcpReciever->localPort();
    temp += QString("服务器:[%1:%2]").arg(ipCli).arg(portCli);
    ui->m_Label->setText(temp);//显示服务器客户端分别地址和ip



    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);
    //使用数据流写入数据
    out.setVersion(QDataStream::Qt_4_6);
    //设置数据流的版本，客户端和服务器端使用的版本要相同
    out<<(quint16) 0;
    //要发送的数据放到out
    out<<tr("1");
    //要发送的数据放到out
    out.device()->seek(0);
    out<<(quint16)(block.size()-sizeof(quint16));
    //要发送的数据放到out

    connect(TcpReciever,SIGNAL(disconnected()),TcpReciever,SLOT(deleteLater()));
    TcpReciever->write(block);

    //TcpReciever->disconnectFromHost();
    ui->statusLabel->setText("send message successful!!!");
    //发送数据成功后，显示提示
}

void UpdtServer::readClient()
{
    //将读取的客户端信息显出来
    TcpReciever->waitForReadyRead();
    QString str = TcpReciever->readAll();
    ui->textEdit->append(str);
}
