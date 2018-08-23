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
    void on_pbtn_clicked();

private:
    Ui::Widget  *ui;
    QTcpServer  *tcpServer;
};

#endif // WIDGET_H
