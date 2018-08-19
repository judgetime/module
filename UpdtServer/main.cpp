#include "UpdtServer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UpdtServer w;
    w.show();

    return a.exec();
}
