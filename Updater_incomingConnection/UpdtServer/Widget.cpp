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
    this->setWindowTitle("服务器");
    ui->pbn_Listen->setEnabled(false);
    ui->lineEdit_Port->setText("8080");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pbn_Listen_clicked()
{
    if(ui->lineEdit_File->text().isEmpty()){
        QMessageBox::information(this,tr("错误"),tr("请选择要更新的文件！"));
        return;
    }
    fileName = ui->lineEdit_File->text();
    QFileInfo file(fileName);
    if(!file.exists()) {
        QMessageBox::information(this,tr("错误"),tr("文件不存在！"));
        return;
    }

    server = new Server(fileName, this);
    int port = ui->lineEdit_Port->text().toInt();
    //server->listen(QHostAddress::Any,port)
    if(!server->listen(QHostAddress::Any,port)) {  //监听本地主机的8080端口，如果出错就输出错误信息，并关闭
        qDebug() << server->errorString();
        server->close();
    }else{
        ui->textEdit->append("开始监听...");
    }
}

void Widget::on_pbn_OpenFile_clicked()
{
    fileName = QFileDialog::getOpenFileName(this,tr("Open File"),tr(""),tr("所有文件 (*.*)"));
    if(fileName == "") {                //如果用户直接关闭了文件浏览对话框，那么文件名就为空值，直接返回
        return;
    } else {
      ui->lineEdit_File->setText(fileName);
    }

    ui->pbn_Listen->setEnabled(true);
}
