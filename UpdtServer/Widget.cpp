#include "Widget.h"
#include "ui_Widget.h"
#include "Server.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pbtn_clicked()
{
    tcpServer = new Server(this);
    if(!tcpServer->listen(QHostAddress::Any,8080)) {  //监听本地主机的8080端口，如果出错就输出错误信息，并关闭
        qDebug() << tcpServer->errorString();
        close();
    }
    ui->textEdit->append("开始监听");
}
