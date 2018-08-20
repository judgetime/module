#include "Appup.h"
#include "ui_Appup.h"
#include "DomDocument.h"

Appup::Appup(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::Appup)
{
    ui->setupUi(this);
    //固定高和宽：
    this->setFixedSize(400,200);
    //设置背景颜色（两种方法都可以）
    //this->setStyleSheet("QMainWindow{background:rgb(240,250,250)}");
    QPalette pal;
    pal.setColor(QPalette::Background,QColor(255,245,225) );
    this->setPalette(pal);
    this->setAutoFillBackground(true);

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
    //获取服务器地址
    QString ipAdd;//服务器地址
    QFile netfile("net.dat");
    if (netfile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&netfile);
        ipAdd = stream.readLine();
        netfile.close();
    }
    //连接到服务器
    clientSocket=new QTcpSocket(0);
    clientSocket->connectToHost("127.0.0.1",8080);
    QObject::connect(clientSocket,SIGNAL(readyRead()),this,SLOT(communicate()));
    QObject::connect(clientSocket,SIGNAL(disconnected()),this,SLOT(disconnect()));
}
Appup::~Appup()
{
    delete ui;
}
void Appup::communicate()
{
//    DomDocument dom("update.xml");
//    QString version;
//    dom.readXml(version);
//    qDebug() << version;

    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_4_6);
    while(1) {
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
                break;
            }
            //总的数据大小
            in >> nextBlockSize;
            //设置进度条最大值
            ui->progressBar->setMaximum((int)nextBlockSize);
            //nextBlockSize以十进制格式显示
            ui->totalLabel->setText(QString::number((int)nextBlockSize));
            this->show();
        }
        if (quint64(bytes) < nextBlockSize) {
            break;
        }
        //通信协议
        quint16 protocol;
        in>>protocol;
        switch(protocol) {
            case 0x0001:
            {
                in>>version_s;
                //最新版本，不用升级
                if(version==version_s) {
                    QMessageBox::information(this,tr("提示"),tr("当前版本已经是最新的了！"));
                    QProcess::startDetached("MRP.exe");
                    qApp->quit();
                }
                else {
                    QByteArray block;
                    QDataStream out(&block, QIODevice::WriteOnly);
                    out.setVersion(QDataStream::Qt_4_6);
                    out<<quint64(0)<<quint16(0x0001);
                    out.device()->seek(0);
                    out<<quint64(block.size() - sizeof(quint64));
                    clientSocket->write(block);
                }
                break;
            }
            case 0x0002: {
                QByteArray block1;
                QByteArray block2;
                in>>block1>>block2;
                //打开要更新的文件并写入数据
                QFile file1("MRP.exe");
                if (file1.open(QIODevice::WriteOnly)&&block1.length()!=0) {
                    file1.write(block1);
                    file1.close();
                }
                QFile file2("common.dll");
                if (file2.open(QIODevice::WriteOnly)&&block2.length()!=0) {
                    file2.write(block2);
                    file2.close();
                }
                //修改版本文件
                QFile wvfile("version.dat");
                if (wvfile.open(QIODevice::WriteOnly)) {
                    wvfile.write(version_s.toUtf8());
                    wvfile.write("/r/n");
                    //写入更新日期
                    wvfile.write(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8());
                    wvfile.close();
                }
                QMessageBox::information(this,tr("升级成功"),"您的软件版本已经从"+version+"升级到"+version_s+"，将重启启动软件！");
                //启动主程序
                QProcess::startDetached("MRP.exe");
                qApp->quit();
                break;
            }
        }
        nextBlockSize = 0;
    }
}
void Appup::disconnect()
{
    clientSocket->deleteLater();
    QMessageBox::information(this,tr("错误"),tr("网络连接中断！"));
    qApp->quit();
}
