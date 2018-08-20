#include "Server.h"

Server::Server(QObject *parent):QTcpServer(parent)
{
    //本地读取要传输的文件
    QFile file("version.dat");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        version = stream.readLine();
        file.close();
    }
    QFile upfile1("MRP.exe");
    QFile upfile2("common.dll");
    if (upfile1.open(QIODevice::ReadOnly) && upfile2.open(QIODevice::ReadOnly)) {
        file1 = upfile1.readAll();
        file2 = upfile2.readAll();
    }
}
