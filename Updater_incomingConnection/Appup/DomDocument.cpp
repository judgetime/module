#include "DomDocument.h"
#include <QDebug>

DomDocument::DomDocument(QString fileName) :
    fileName(fileName)
{

}

DomDocument::~DomDocument()
{

}

bool DomDocument::readVersion(QString &version)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
            std::cerr << "Error: Cannot read file "  << qPrintable(fileName)
                      << ": " << qPrintable(file.errorString())
                      << std::endl;
            return false;
    }
    QString errorStr = "";
    int errorLine = 0;
    int errorColumn = 0;
    QDomDocument doc;
    if (!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn)) {
        std::cerr << "Error: Parse error at line " << errorLine << ", "
                  << "column " << errorColumn << ": "
                  << qPrintable(errorStr) << std::endl;
        return false;
    }
    file.close();

    QDomElement root = doc.documentElement();
    //qDebug() << root.tagName();
    if (root.tagName() != "SoftwareInfo") {
        std::cerr << "Error: Not a SoftwareInfo file" << std::endl;
        return false;
    }
    QDomNode node = root.firstChild();
    while(!node.isNull()) {
        QDomElement element = node.toElement(); // try to convert the node to an element.
        if(!element.isNull()) {
            //qDebug()<<element.tagName() << ":" << element.text();
            if(element.tagName() == "Version") {
                version = element.text();
            }
        }
        node = node.nextSibling();
    }
    return true;
}
bool DomDocument::readAppName(QString &version)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
            std::cerr << "Error: Cannot read file "  << qPrintable(fileName)
                      << ": " << qPrintable(file.errorString())
                      << std::endl;
            return false;
    }
    QString errorStr = "";
    int errorLine = 0;
    int errorColumn = 0;
    QDomDocument doc;
    if (!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn)) {
        std::cerr << "Error: Parse error at line " << errorLine << ", "
                  << "column " << errorColumn << ": "
                  << qPrintable(errorStr) << std::endl;
        return false;
    }
    file.close();

    QDomElement root = doc.documentElement();
    if (root.tagName() != "SoftwareInfo") {
        std::cerr << "Error: Not a SoftwareInfo file" << std::endl;
        return false;
    }
    QDomNode node = root.firstChild();
    while(!node.isNull()) {
        QDomElement element = node.toElement(); // try to convert the node to an element.
        if(!element.isNull()) {
            if(element.tagName() == "SoftwareName") {
                version = element.text();
            }
        }
        node = node.nextSibling();
    }
    return true;
}
