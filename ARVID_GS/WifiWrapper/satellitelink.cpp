#include "satellitelink.h"
#include <QNetworkInterface>
#include <QDateTime>
#include <QtEndian>

//SatelliteLink::SatelliteLink(QObject *parent, bool checkChecksum) : QObject(parent), localAddress("192.168.0.100"), remoteAddress("192.168.0.101"), port(2000), socket(this), bound(false), checkChecksum(checkChecksum), receivedBytes(0), sentBytes(0), timer(this){

SatelliteLink::SatelliteLink(QObject *parent, QHostAddress localAddress, QHostAddress remoteAddress, quint16 port, bool checkChecksum) : QObject(parent), localAddress(localAddress), remoteAddress(remoteAddress), port(port), socket(this), bound(false), checkChecksum(checkChecksum), receivedBytes(0), sentBytes(0), timer(this){

    qDebug() << "Binding to IP" << localAddress.toString() << "and port" << port << "\n";
    if(socket.bind(localAddress, port)){
        qDebug() << "Bind successful!\n";
        bound = true;
    }else{
        qDebug()<< "Bind unsuccessful!\n";
        return;
    }
    connect(&socket, SIGNAL(readyRead()), this, SIGNAL(readReady()));
}

Payload SatelliteLink::read(){
    QByteArray buffer(1023, 0x00);

    receivedBytes = socket.readDatagram(buffer.data(), buffer.size());

    //qDebug() << "Received Bytes: " << receivedBytes << endl;

    Payload payload(buffer);

    quint32 checksum = 0;
    for(int i = 2; i < 26 + payload.userDataLen; ++i){
        if (checksum & 01)
            checksum = checksum >> 1 | 0x8000;
        else
            checksum >>= 1;

        checksum += (quint8)buffer[i];
        checksum &= 0xFFFF;
    }

    if((!checkChecksum || checksum == payload.checksum) && topics.contains(payload.topic))
        return payload;
    else
        return Payload();
}

void SatelliteLink::addTopic(PayloadType topicId){
    topics.insert(topicId);
}

int SatelliteLink::write(quint32 topicId, const QByteArray &data){

    //qDebug()<<"2nd write is called"<<endl;
    QByteArray buffer(1023, 0x00);

    *((quint32*)(buffer.data() + 2)) = qToBigEndian((quint32)1);
    *((quint64*)(buffer.data() + 6)) = qToBigEndian((quint64)QDateTime::currentDateTime().toMSecsSinceEpoch() * 1000000);
    *((quint32*)(buffer.data() + 14)) = qToBigEndian((quint32)1);
    *((quint32*)(buffer.data() + 18)) = qToBigEndian((quint32)topicId);
    *((quint16*)(buffer.data() + 22)) = qToBigEndian((quint16)10);
    *((quint16*)(buffer.data() + 24)) = qToBigEndian((quint16)data.length());
    memcpy(buffer.data() + 26, data.constData(), data.length());
    *(buffer.data() + 26 + data.length()) = 0x00;

    quint32 checksum = 0;
    for(int i = 2; i < 26 + data.length(); ++i){
        if (checksum & 01)
            checksum = checksum >> 1 | 0x8000;
        else
            checksum >>= 1;

        checksum += (quint8)buffer[i];
        checksum &= 0xFFFF;
    }
    *((quint16*)(buffer.data() + 0)) = qToBigEndian((quint16)checksum);

    int extra = 0;

    buffer.resize(26 + data.length() + extra);

    int written = socket.writeDatagram(buffer.constData(), buffer.size(), remoteAddress, port);
    sentBytes += written;
    return written;
}


/*int SatelliteLink::write(quint32 topicId, const Telecommand &telecommand)
{
    qDebug()<<"Simple write is called"<<endl;
    QByteArray buffer(sizeof(Telecommand), 0x00);
    memcpy(buffer.data(), (char*)&telecommand, sizeof(Telecommand));
    return write(topicId, buffer);


} */

qint64 SatelliteLink::readAndResetReceivedBytes(){
    unsigned ret = receivedBytes;
    receivedBytes = 0;
    return ret;
}

qint64 SatelliteLink::readAndResetSentBytes(){
    unsigned ret = sentBytes;
    sentBytes = 0;
    return ret;
}

bool SatelliteLink::isBound(){
    return bound;
}
