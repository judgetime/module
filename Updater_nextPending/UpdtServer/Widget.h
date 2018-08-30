#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QByteArray>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

signals:

private slots:
    void on_pbtn_clicked();
    void on_pbn_File_clicked();
    void newConnectionSlot();
    void sendDataSlot(QTcpSocket* socket, QByteArray block);

private:
    Ui::Widget  *ui;
    QTcpServer  *server;
    QString     fileName;
    QString     version;
    QByteArray  file1, file2;
};

#endif // WIDGET_H
