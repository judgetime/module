#include "Appup.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Appup w;
    w.show();

    return a.exec();
}
