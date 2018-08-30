#ifndef DOMDOCUMENT_H
#define DOMDOCUMENT_H

#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <iostream>
using namespace std;

class DomDocument
{
public:
    DomDocument(QString fileName);
    ~DomDocument();
    bool writeXml();
    bool readXml(QString &version);
    bool updateXml(QString version);

private:
    QString fileName;
};

#endif // DOMDOCUMENT_H
