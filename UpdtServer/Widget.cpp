#include "Widget.h"
#include "ui_Widget.h"
#include "SocketManager.h"
#include "Sockethandle.h"
#include "DomDocument.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    server = new QTcpServer(this);
    connect (server, SIGNAL(newConnection()), this, SLOT(newConnectionSlot()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pbtn_clicked()
{
    if(ui->lineEdit_File->text().isEmpty()){
        QMessageBox::information(this,tr("错误"),tr("请选择要更新的文件！"));
        return;
    }
    if(ui->lineEdit_File->text() != 0) {
        fileName = ui->lineEdit_File->text();
    }
    QFileInfo file(fileName);
    if(!file.exists()) {
        QMessageBox::information(this,tr("错误"),tr("文件不存在！"));
        return;
    }

    //tcpServer = new Server(file1, file2, this);
    //server = new QTcpServer(this);
    if(!server->listen(QHostAddress::Any,8080)) {  //监听本地主机的8080端口，如果出错就输出错误信息，并关闭
        qDebug() << server->errorString();
        server->close();
    }else{
        ui->textEdit->append("开始监听...");
    }
}

void Widget::on_pbn_File_clicked()
{
    fileName = QFileDialog::getOpenFileName(this,tr("Open File"),tr(""),tr("所有文件 (*.*)"));
    if(fileName == "") {                //如果用户直接关闭了文件浏览对话框，那么文件名就为空值，直接返回
        return;
    } else {
      ui->lineEdit_File->setText(fileName);
    }    

    QFile upfile1("update.xml");
    QFile upfile2(fileName);
    if (upfile1.open(QIODevice::ReadOnly) && upfile2.open(QIODevice::ReadOnly)) {
        file1 = upfile1.readAll();
        file2 = upfile2.readAll();
        upfile1.close();
        upfile2.close();
    }
    QString filename("update.xml");
    DomDocument dom(filename);
    dom.readXml(version);
    qDebug() << "版本：" << version;
}

void Widget::newConnectionSlot ()
{
    QTcpSocket *clientSocket = server->nextPendingConnection ();
    QString address = clientSocket->peerAddress ().toString ();
    QString port = QString::number (clientSocket->peerPort ());
    QString clientInfo = "ip: " + address  + ", port: " + port;
    ui->textEdit->append (clientInfo + "  链接上服务器....");
    qDebug() << "主线程"<< __FUNCTION__ << QThread::currentThreadId();

    Sockethandle * sh = new Sockethandle(clientSocket, version, file1, file2);
    QThread * thread = new QThread(this);
    sh->moveToThread(thread);//把tcp类移动到新的线程
    thread->start();
    SocketManager *instance = SocketManager::getInstance();
    instance->insertClient(sh);
    connect (sh, SIGNAL(sendData(QTcpSocket*,QByteArray)),
             this , SLOT(sendDataSlot(QTcpSocket*,QByteArray)),
             Qt::BlockingQueuedConnection);
    connect(clientSocket, SIGNAL(disconnected()), thread, SLOT(quit()));//断开连接时线程退出
    connect(thread, SIGNAL(finished()), sh, SLOT(deleteLater()));
}

void Widget::sendDataSlot(QTcpSocket* socket, QByteArray block)
{
    socket->write(block);
    socket->flush();
    qDebug() << "发送数据";
}
