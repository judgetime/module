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

private:
    Ui::Widget  *ui;
    QTcpServer  *tcpServer;
    QByteArray  file1;
    QByteArray  file2;
    QString     fileName;
};

#endif // WIDGET_H
