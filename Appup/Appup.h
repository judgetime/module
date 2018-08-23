#ifndef APPUP_H
#define APPUP_H
#include <QDialog>
#include <QMessageBox>
#include <QFile>
#include <QTcpSocket>
#include <QDataStream>
#include <QProcess>
#include <QDatetime>

namespace Ui {
class Appup;
}

class Appup : public QDialog
{
    Q_OBJECT

public:
    explicit Appup(QWidget *parent = 0);
    ~Appup();
private slots:
    void disconnect();
    void readDataSlot();
    void on_pbn_connect_clicked();

private:
    QTcpSocket *clientSocket;
    quint64 nextBlockSize;
    QString version,version_s;
    int bytesWritten;
    Ui::Appup *ui;
};

#endif // APPUP_H
