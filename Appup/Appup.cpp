#include "Appup.h"
#include "ui_Appup.h"
#include "DomDocument.h"
#include <QFileDialog>

Appup::Appup(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::Appup)
{
    ui->setupUi(this);

    ui->lineEdit_IP->setText("127.0.0.1");
    ui->lineEdit_Port->setText("8080");
    //固定高和宽：
    //this->setFixedSize(400,200);
    //设置背景颜色
    this->setStyleSheet("QMainWindow{background:rgb(240,250,250)}");

    nextBlockSize = 0;
    bytesWritten = 0;
    ui->progressBar->setMaximum(0);
    //获得本地版本号
//    QFile vfile("version.dat");
//    if (vfile.open(QIODevice::ReadOnly)) {
//        QTextStream stream(&vfile);
//        version = stream.readLine();
//        vfile.close();
//    }
    QString fileName("update.xml");
    DomDocument dom(fileName);
    dom.readXml(version);
    qDebug() << "版本：" << version;


}
Appup::~Appup()
{
    delete clientSocket;
    delete ui;
}
void Appup::readDataSlot()
{
    if( clientSocket->bytesAvailable() <= 0 ) {
      return;
    }
    qDebug() << "收到服务器响应...";
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_5_8);
    //while(1) {
        //获取当前已经获取的数据的大小
        int bytes=(int)(clientSocket->bytesAvailable());
        //已经接收
        bytesWritten += bytes;
        if(ui->progressBar->maximum()) {
            //设置进度条当前进度
            ui->progressBar->setValue(bytes);
            ui->comeLabel->setText(QString::number(bytes));
        }
        if (nextBlockSize == 0) {
            if (quint64(bytes) < sizeof(quint64)) {
                qDebug() << "没有文件大小。:";
                //break;
                return;
            }
            //总的数据大小
            in >> nextBlockSize;
            qDebug() << "接受的数据：" << nextBlockSize;
            //设置进度条最大值
            ui->progressBar->setMaximum((int)nextBlockSize);
            //nextBlockSize以十进制格式显示
            ui->totalLabel->setText(QString::number((int)nextBlockSize));
            this->show();
        }
        if (quint64(bytes) < nextBlockSize) {
            qDebug() << "没有得到全部的数据.:";
            //break;
            return;
        }
        /*通信协议:
         *      客户端接收：
         *              0x0001表示服务器发送过来版本号，在客户端本地进行比对
         *              0x0002表示服务器将最新程序的xml文件和二进制文件发送过来
         * QDataStream的格式为：数据长度（quint64）,通信协议（quint16）,具体内容
         */
        quint16 protocol;
        in >> protocol;
        qDebug() << "客户端protocol:" << QString::number(protocol,16);
        switch(protocol) {
            case 0x0001: {
                in >> version_s;
                qDebug() << "最新版本：" << version_s;
                //最新版本，不用升级
                if(version == version_s) {
                    QMessageBox::information(this,tr("提示"),tr("当前版本已经是最新的了！"));
                    //QProcess::startDetached("MRP.exe");
                    //qApp->quit();
                    QByteArray temp_buf;
                    temp_buf = clientSocket->readAll();
                    qDebug() << "客户端接收剩余数据：" << temp_buf;
                    return;
                }
                else {
                    in >> version_s;
                    qDebug() << "不是最新版本，请求服务器更新。";
                    QByteArray block;
                    QDataStream out(&block, QIODevice::WriteOnly);
                    out.setVersion(QDataStream::Qt_5_8);
                    out << quint64(0);
                    out << quint16(0x0002) << version;
                    out.device()->seek(0);
                    out << quint64(block.size() - sizeof(quint64));
                    clientSocket->write(block);
                    //clientSocket->flush();
                    qDebug() << "发送数据";
                    qDebug() << block.size() - sizeof(quint64);
                }
                //break;
                return;
            }
            case 0x0002: {
                qDebug() << "开始更新...";
                QByteArray block1;
                QByteArray block2;
                in>>block1>>block2;
                //打开要更新的文件并写入数据
                QFile file1("MRP.xml");
                if (file1.open(QIODevice::WriteOnly)&&block1.length()!=0) {
                    file1.write(block1);
                    file1.close();
                }
                QFile file2("update.xml");
                if (file2.open(QIODevice::WriteOnly)&&block2.length()!=0) {
                    file2.write(block2);
                    file2.close();
                }
                //修改版本文件
//                QFile wvfile("version.dat");
//                if (wvfile.open(QIODevice::WriteOnly)) {
//                    wvfile.write(version_s.toUtf8());
//                    wvfile.write("/r/n");
//                    //写入更新日期
//                    wvfile.write(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8());
//                    wvfile.close();
//                }
                QMessageBox::information(this,tr("升级成功"),"您的软件版本已经从"+version+"升级到"+version_s+"，将重启启动软件！");
                //启动主程序
                QProcess::startDetached("MRP.exe");
                qApp->quit();
                //break;
                return;
            }
        }
        nextBlockSize = 0;
    //}
}
void Appup::disconnect()
{
    clientSocket->deleteLater();
    QMessageBox::information(this,tr("错误"),tr("网络连接中断！"));
    qApp->quit();
}

void Appup::on_pbn_connect_clicked()
{
    if(ui->pbn_connect->text() == tr("连接服务器")) {
        if(ui->lineEdit_IP->text().isEmpty()) {
            QMessageBox::information(this,tr("错误"),tr("未设置IP！"));
            return;
        }
        if(ui->lineEdit_Port->text().isEmpty()) {
            QMessageBox::information(this,tr("错误"),tr("未设置端口号！"));
            return;
        }
        QString ip = ui->lineEdit_IP->text();
        int port = ui->lineEdit_Port->text().toInt();
        //连接到服务器
        clientSocket= new QTcpSocket(this);
        //取消已有的连接
        clientSocket->abort();
        clientSocket->connectToHost(ip,port);
        //等待连接成功
        if(!clientSocket->waitForConnected(30000)) {
            ui->textEdit->append("连接服务器失败！");
            return;
        }
        ui->textEdit->append("连接服务器成功！");
        ui->pbn_connect->setText("断开连接");
        QObject::connect(clientSocket,SIGNAL(readyRead()),this,SLOT(readDataSlot()));
        QObject::connect(clientSocket,SIGNAL(disconnected()),this,SLOT(disconnect()));
    }else{
        //如果正在连接（点击侦听后立即取消侦听，若socket没有指定对象会有异常）
        if(clientSocket->state() == QAbstractSocket::ConnectedState) {
            //关闭连接
            clientSocket->disconnectFromHost();
        }
        //取消侦听
        clientSocket->close();
        ui->textEdit->append("断开连接...");
        ui->pbn_connect->setText("连接服务器");
    }
}

void Appup::on_pbn_Check_clicked()
{
    //发送当前服务器上的客户端版本信息给升级客户端
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
    out.setVersion(QDataStream::Qt_5_8);//设置数据流的版本，客户端和服务器端使用的版本要相同
    out << quint64(0) << quint16(0x0001) << version;//要发送的数据放到out
    out.device()->seek(0);
    out << (quint64)(block.size() - sizeof(quint64));//要发送的数据放到out
    qDebug() << "发送数据block.size():" << block.size() << "sizeof(quint64):" << sizeof(quint64);
    clientSocket->write(block);
    clientSocket->flush();
}
