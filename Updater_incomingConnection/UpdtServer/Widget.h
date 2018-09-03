#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pbn_OpenFile_clicked();

    void on_pbn_Listen_clicked();

private:
    Ui::Widget  *ui;
    QTcpServer  *server;
    QString     fileName;
    QString     version;
    QByteArray  file1, file2;
};

#endif // WIDGET_H
