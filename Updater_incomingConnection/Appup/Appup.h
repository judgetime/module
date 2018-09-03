#ifndef APPUP_H
#define APPUP_H
#include <QDialog>

namespace Ui {
class Appup;
}

class QTcpSocket;

class Appup : public QDialog
{
    Q_OBJECT

public:
    explicit Appup(QWidget *parent = 0);
    ~Appup();
    void createFileTemp();
    bool progressHandle();
private slots:
    void on_pbn_connect_clicked();
    void on_pbn_checkServerVersion_clicked();
    void disconnect();
    void readDataSlot();
    void on_pbn_checkLocalVersion_clicked();

private:
    Ui::Appup*  ui;
    QTcpSocket* clientSocket;
    QString     version;        //版本
    QString     appName;        //程序名
    QString     updater;        //update.xml
    int         bytesWritten;
    quint64     nextBlockSize;
};

#endif // APPUP_H
