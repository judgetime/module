#include "Appup.h"
#include "ui_Appup.h"
#include "DomDocument.h"
#include <QMessageBox>
#include <QFile>
#include <QTcpSocket>
#include <QDataStream>
#include <QProcess>
#include <QDatetime>
#include <QHostAddress>
#include <QDir>

Appup::Appup(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::Appup)
{
    ui->setupUi(this);
    this->setWindowTitle("客户端");
    ui->lineEdit_IP->setText("127.0.0.1");
    ui->lineEdit_Port->setText("8080");
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);
    ui->pbn_checkLocalVersion->setEnabled(false);
    ui->pbn_checkServerVersion->setEnabled(false);
    ui->textEdit->setEnabled(false);

    appName = "app";
    updater = "update.xml";
    //设置程序开机自启
    //system("boot.sh");

    nextBlockSize = 0;
}
Appup::~Appup()
{
//    clientSocket->close();
//    delete clientSocket;
    delete ui;
}

void Appup::on_pbn_connect_clicked()
{
    if(ui->pbn_connect->text() ==  "连接服务器") {
        if(ui->lineEdit_IP->text().isEmpty()) {
            QMessageBox::information(this, "错误", "未设置IP！");
            return;
        }
        if(ui->lineEdit_Port->text().isEmpty()) {
            QMessageBox::information(this, "错误", "未设置端口号！");
            return;
        }
        QString ip = ui->lineEdit_IP->text();
        int port = ui->lineEdit_Port->text().toInt();

        clientSocket= new QTcpSocket(this);
        clientSocket->abort();
        //clientSocket->connectToHost(QHostAddress::LocalHost,port);
        clientSocket->connectToHost(ip,port);
        if(!clientSocket->waitForConnected(30000)) {
            ui->textEdit->append("连接服务器失败！");
            return;
        }else{
            ui->textEdit->append("连接服务器成功！");
            ui->pbn_checkLocalVersion->setEnabled(true);
            ui->pbn_checkServerVersion->setEnabled(true);
            qDebug() << "连接服务器。。。";
            ui->pbn_connect->setText("断开连接");
            QObject::connect(clientSocket,SIGNAL(readyRead()),this,SLOT(readDataSlot()));
            QObject::connect(clientSocket,SIGNAL(disconnected()),this,SLOT(disconnect()));

            DomDocument dom(updater);
            dom.readVersion(version);
            dom.readAppName(appName);
            qDebug() << "版本：" << version << "程序名：" << appName;
        }
    }else{
        //如果正在连接（点击侦听后立即取消侦听，若socket没有指定对象会有异常）
        if(clientSocket->state() == QAbstractSocket::ConnectedState) {
            clientSocket->disconnectFromHost();//关闭连接
        }
        clientSocket->close();//取消侦听
        ui->textEdit->append("断开连接...");
        ui->pbn_connect->setText("连接服务器");
    }
}

void Appup::on_pbn_checkServerVersion_clicked()
{
    //发送当前服务器上的客户端版本信息给升级客户端
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
    out.setVersion(QDataStream::Qt_5_8);//设置数据流的版本，客户端和服务器端使用的版本要相同
    out << quint64(0) << quint16(0x0001);//要发送的数据放到out
    out.device()->seek(0);
    out << (quint64)(block.size() - sizeof(quint64));//要发送的数据放到out
    qDebug() << "第一次发送数据block.size():" << block.size() << "sizeof(quint64):" << sizeof(quint64);
    clientSocket->write(block);
    //clientSocket->flush();
}
void Appup::on_pbn_checkLocalVersion_clicked()
{
    QString str("本地软件版本：" + version);
    ui->textEdit->append(str);
}

void Appup::disconnect()
{
    clientSocket->deleteLater();
    QMessageBox::information(this,"错误", "网络连接中断！");
    //qApp->quit();
}

/*
 * 通信协议:
 *      客户端接收：
 *              0x0001表示服务器发送过来版本号，在客户端本地进行比对
 *              0x0002表示服务器将最新程序的xml文件和二进制文件发送过来
 * QDataStream的格式为：数据长度（quint64）,通信协议（quint16）,具体内容
 */
void Appup::readDataSlot()
{
    //循环的作用是当数据过大需要多次接收时，循环判断数据是否接收完整
    //while( clientSocket->bytesAvailable() > 0 ) {
        QString version_s("");
        QDataStream in(clientSocket);
        in.setVersion(QDataStream::Qt_5_8);
        int bytes=(int)(clientSocket->bytesAvailable());
        qDebug() << "获取当前已经获取的数据的大小" << bytes;
        bytesWritten += bytes;
        if(ui->progressBar->maximum()) {
            ui->progressBar->setValue(bytes);//设置进度条当前进度
            ui->comeLabel->setText(QString::number(bytes));
        }
        if (nextBlockSize == 0) {
            if (quint64(bytes) < sizeof(quint64)) {
                qDebug() << "没有文件大小。:";
//                break;
                return;
            }
            in >> nextBlockSize;    //总的数据大小
            qDebug() << "接受的数据：" << nextBlockSize;
            ui->progressBar->setMaximum((int)nextBlockSize);//设置进度条最大值
            ui->progressBar->setValue((int)bytesWritten);
            ui->totalLabel->setText(QString::number((int)nextBlockSize));//nextBlockSize以十进制格式显示
            this->show();
        }
        if (quint64(bytes) < nextBlockSize) {
            qDebug() << bytes << "没有得到全部的数据。";
//            break;
            return;
        }
        quint16 protocol;
        in >> protocol;
        switch(protocol) {
        case 0x0001: {
            in >> version_s;
            QString text = "最新版本：" + version_s;
            ui->textEdit->append(text);
            //最新版本，不用升级
            if(version == version_s) {
                QMessageBox::information(this, "提示", "当前版本已经是最新的了！");
            } else {
                if(QMessageBox::warning(this, "warning", "是否现在更新？",
                                        QMessageBox::Yes | QMessageBox::Default,
                                        QMessageBox::No | QMessageBox::Escape )
                    == QMessageBox::No){
                    break;
                }
                ui->textEdit->append("当前版本不是最新版本，请求服务器更新。");
                QByteArray block;
                QDataStream out(&block, QIODevice::WriteOnly);
                out.setVersion(QDataStream::Qt_5_8);
                out << quint64(0) << quint16(0x0002);
                out.device()->seek(0);
                out << quint64(block.size() - sizeof(quint64));
                clientSocket->write(block);
                clientSocket->flush();
                qDebug() << "发送数据" << block.size() - sizeof(quint64);
            }
            break;
        }
        case 0x0002: {
            createFileTemp();
            QString appPath = "Temp/" + appName;
            QString updaterPath = "Temp/" + updater;
            qDebug() << appPath << updaterPath;
            in >> version_s;
            qDebug() << "开始更新版本" << version_s;
            QByteArray block1;
            QByteArray block2;
            in >> block1 >> block2;

            QFile file1(appPath);//打开要更新的文件并写入数据
            if (file1.open(QIODevice::WriteOnly)&&block1.length() != 0) {
                file1.write(block1);
                file1.close();
            }
            QFile file2(updaterPath);
            if (file2.open(QIODevice::WriteOnly)&&block2.length() != 0) {
                file2.write(block2);
                file2.close();
            }
            QMessageBox::information(this, "升级成功", "您的软件版本已经从"+version+"升级到"+version_s+"，将重启启动软件！");
            if(QMessageBox::warning(this, "warning", "是否立即替换成最新版本？",
                                    QMessageBox::Yes | QMessageBox::Default,
                                    QMessageBox::No | QMessageBox::Escape )
                == QMessageBox::No){
                break;
            }
            progressHandle();
            break;
        }
        default:
            break;
        }
        nextBlockSize = 0;
    //}
}
void Appup::createFileTemp()
{
    QString workPath = QDir::currentPath();
    qDebug() << workPath;
    QDir dir(workPath);
    //dir.cd("../path");            //进入某文件夹
    if(!dir.exists("Temp")){        //判断需要创建的文件夹是否存在
        dir.mkdir("Temp");          //创建文件夹
    }else{
        ui->textEdit->append( "临时文件夹已存在。");
    }
}
bool Appup::progressHandle()
{
    QString workPath = QDir::currentPath();
    QString cmdKill = "pkill " + appName;
//    QString cmdDelApp = workPath + "/" + appName;
    QString cmdDelUpdater = workPath + "/" + updater;
    QString appOldPath = workPath + "/Temp/" + appName;
    QString appNewPath = workPath + "/" + appName;
    QString UpdaterOldPath = workPath + "/Temp/" + updater;
    QString UpdaterNewPath = workPath + "/" + updater;
    QString TempPath = workPath + "/Temp";

    QProcess::execute(cmdKill);             //先关闭程序
    QFile file1(appName), file2(updater);
    if(!file1.exists())
        file1.remove();
    if(!file2.exists())
        file2.remove();
    QFile::copy(appOldPath, appNewPath);
    QFile::copy(UpdaterOldPath, UpdaterNewPath);
    QFile::remove(TempPath);
    QProcess::startDetached(appName);//启动主程序
    qDebug() << __FUNCTION__ << TempPath << cmdDelUpdater;
    //qApp->quit();
    return true;
}
