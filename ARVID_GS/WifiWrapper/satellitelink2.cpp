#include "satellitelink2.h"
#include <QNetworkInterface>
#include <QDateTime>
#include <QtEndian>

//Satellitelink2::Satellitelink2(QObject *parent, bool checkChecksum) : QObject(parent), localAddress("192.168.0.100"), remoteAddress("192.168.0.101"), port(2000), socket(this), bound(false), checkChecksum(checkChecksum), receivedBytes(0), sentBytes(0), timer(this){

Satellitelink2::Satellitelink2(QObject *parent, bool checkChecksum) : QObject(parent), localAddress("172.17.69.91"), remoteAddress("172.17.69.87"), port(5000), socket(this), bound(false), checkChecksum(checkChecksum), receivedBytes(0), sentBytes(0), timer(this){

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

Payload Satellitelink2::read(){
    QByteArray buffer(1023, 0x00);
    receivedBytes += socket.readDatagram(buffer.data(), buffer.size());

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

    qDebug() << "Topic ID: " << payload.topic << endl;

    if((!checkChecksum || checksum == payload.checksum) && topics.contains(payload.topic))
        return payload;
    else
        return Payload();
}

void Satellitelink2::addTopic(PayloadType topicId){
    topics.insert(topicId);
}

int Satellitelink2::write(quint32 topicId, const QByteArray &data){

    qDebug()<<"2nd write is called"<<endl;
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


int Satellitelink2::write(quint32 topicId, const Telecommand &telecommand)
{
    qDebug()<<"Simple write is called"<<endl;
    QByteArray buffer(sizeof(Telecommand), 0x00);
    memcpy(buffer.data(), (char*)&telecommand, sizeof(Telecommand));
    return write(topicId, buffer);


}

qint64 Satellitelink2::readAndResetReceivedBytes(){
    unsigned ret = receivedBytes;
    receivedBytes = 0;
    return ret;
}

qint64 Satellitelink2::readAndResetSentBytes(){
    unsigned ret = sentBytes;
    sentBytes = 0;
    return ret;
}

bool Satellitelink2::isBound(){
    return bound;
}
