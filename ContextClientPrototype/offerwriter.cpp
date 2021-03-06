#include "offerwriter.h"

#include <QDebug>
#include <QDateTime>

OfferWriter::OfferWriter()
{

}

bool OfferWriter::write(QIODevice *device, AbstractWidget *widget) {
    qDebug() << "OfferWriter::write(QIODevice *device, AbstractWidget *widget)";

    // Initialize
    xml.setDevice(device);

    AbstractWriter::write(device, widget);

    CodingWidget* codingWidget = ((CodingWidget*)widget);

    xml.writeDTD("<!DOCTYPE ContextOffer SYSTEM \"ContextOffer.dtd\">");


    // Context Offer
    xml.writeStartElement("ContextOffer");
    xml.writeAttribute("name", codingWidget->getOfferName());
    xml.writeAttribute("createDate", QDateTime::currentDateTime().toUTC().toString(Qt::ISODate));
    xml.writeAttribute("language", codingWidget->getCodingLanguage());


    // ContextHeader

    // UUID - Generate
    uuid_t uuid;
    uuid_generate(uuid);

    // UUID - QString
    QString uuidString;
    size_t s;
    for (s = 0; s < sizeof(uuid) - 1; s++) {
        uuidString += QString("%1").arg(uuid[s], 0, 16);
    }

    xml.writeEmptyElement("ContextHeader");
    xml.writeAttribute("serviceName", codingWidget->getCurrentServiceName());
    xml.writeAttribute("service", QString("%1").arg(codingWidget->getCurrentServiceNumber()));
    xml.writeAttribute("version", "0");
    xml.writeAttribute("channel", "0");
    xml.writeAttribute("optionalSize", "0");
    xml.writeAttribute("uuid", uuidString);
    xml.writeAttribute("ip_address", "Not yet");


    // Context Datagram
    xml.writeStartElement("ContextDatagram");
    xml.writeAttribute("listSize", QString("%1").arg(codingWidget->getCodingElementSize()*2));
    xml.writeAttribute("type", codingWidget->getCodingId());

//    qDebug() << "OfferWriter::write: getContextDatagramList().size(): " << codingWidget->getContextDatagramList().size();
    for(int i=0; i<codingWidget->getContextDatagramList().size(); i++) {
        contextDatagram = codingWidget->getContextDatagramList().at(i);

        xml.writeStartElement("CodingElement");
        xml.writeAttribute("type", contextDatagram.at(1).toString());
        xml.writeAttribute("category", contextDatagram.at(2).toString());
        xml.writeAttribute("selection", contextDatagram.at(9).toString().toUtf8());
        xml.writeAttribute("content", contextDatagram.at(10).toString());
        xml.writeAttribute("mask", contextDatagram.at(11).toString());
        xml.writeAttribute("brickCount", contextDatagram.at(3).toString());
        xml.writeAttribute("name", contextDatagram.at(4).toString());
        xml.writeAttribute("sourceType", contextDatagram.at(5).toString());
        xml.writeAttribute("sourceLocation", contextDatagram.at(6).toString());
        xml.writeAttribute("sourceFormat", contextDatagram.at(7).toString());
        xml.writeAttribute("source", contextDatagram.at(8).toString());

        xml.writeStartElement("Name");
        xml.writeCDATA(contextDatagram.at(9).toString());
        xml.writeEndElement();

        for(int j=10; j<contextDatagram.size(); j+=2) {
            xml.writeStartElement("ContextBrick");
            xml.writeAttribute("content", contextDatagram.at(j).toString());
            xml.writeAttribute("mask", contextDatagram.at(j+1).toString());
            xml.writeEndElement();
        }

        xml.writeEndElement();
    }
    xml.writeEndElement();


    // Context Data
    xml.writeStartElement("ContextData");
    xml.writeAttribute("type", codingWidget->getDataType());
    xml.writeCDATA(codingWidget->getDataText());


    // Close all open tags
    xml.writeEndDocument();

    return true;
}
