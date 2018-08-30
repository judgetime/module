#include "DomDocument.h"
#include <QDebug>

DomDocument::DomDocument(QString fileName) :
    fileName(fileName)
{

}

DomDocument::~DomDocument()
{

}
bool DomDocument::writeXml()

{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    QDomDocument doc;
    QDomText text;
    QDomElement element;
    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" );
    doc.appendChild( instruction );
    QDomElement root = doc.createElement( "SoftwareInfo" );
    doc.appendChild(root);
    element = doc.createElement( "Version" );
    text = doc.createTextNode( "1.0" );
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement( "SoftwareName" );
    text = doc.createTextNode( "Notepad" );
    element.appendChild(text);
    root.appendChild(element);
    QTextStream out(&file);
    doc.save(out, 4);       //each line space of file is 4
    return true;
}
bool DomDocument::readXml(QString &version)
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

bool DomDocument::updateXml(QString version)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "open for do erro";
        file.close();
    }
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        qDebug() << "setcontent for do error";
        file.close();
    }
    file.close();
    QDomElement root = doc.documentElement();
    qDebug() << root.tagName();
    if (root.tagName() != "SoftwareInfo") {
        std::cerr << "Error: Not a SoftwareInfo file" << std::endl;
        return false;
    }
    QDomNode node = root.firstChild();
    while(!node.isNull()) {
        QDomElement element = node.toElement(); // try to convert the node to an element.
        if(!element.isNull()) {
            qDebug()<<element.tagName() << ":" << element.text();
            if(element.nodeName() == "Version") {
//                QDomNode oldnode = element.firstChild();
                element.firstChild().setNodeValue(version);
//                qDebug()<<"after change,the value is"<<element.toElement().text();
//                QDomNode newnode = element.firstChild();
//                element.replaceChild(newnode,oldnode);
            }
        }
        node = node.nextSibling();
    }
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "open for remove error!";
    }
    QTextStream out(&file);
    doc.save(out,4);
    file.close();
    return true;
}
