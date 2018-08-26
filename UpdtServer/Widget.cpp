#include "Widget.h"
#include "ui_Widget.h"
#include "Server.h"
#include <QFileDialog>
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    file1 = 0;
    file2 = 0;
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
    //本地读取要传输的文件
//    QFile file("update.xml");
//    if (file.open(QIODevice::ReadOnly)) {
//        QTextStream stream(&file);
//        version = stream.readLine();
//        file.close();
//    }
    QFile upfile1("update.xml");
    QFile upfile2(fileName);
    if (upfile1.open(QIODevice::ReadOnly) && upfile2.open(QIODevice::ReadOnly)) {
        file1 = upfile1.readAll();
        file2 = upfile2.readAll();
    }

    tcpServer = new Server(file1, file2, this);
    if(!tcpServer->listen(QHostAddress::Any,8080)) {  //监听本地主机的8080端口，如果出错就输出错误信息，并关闭
        qDebug() << tcpServer->errorString();
        close();
    }
    ui->textEdit->append("开始监听");
}

void Widget::on_pbn_File_clicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,tr("Open File"),tr(""),tr("所有文件 (*.*)"));
    if(fileName == "") {                //如果用户直接关闭了文件浏览对话框，那么文件名就为空值，直接返回
        return;
    } else {
      ui->lineEdit_File->setText(fileName);
    }
}
