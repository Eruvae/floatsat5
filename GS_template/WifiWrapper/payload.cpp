#include <QByteArray>
#include <QtEndian>
#include "payload.h"
#include <QDebug>

Payload::Payload() : checksum(0), senderNode(0), timestamp(0), senderThread(0), topic(0), ttl(0), userDataLen(0){
    userData8[0] = 0;
}

Payload::Payload(const QByteArray &buffer) : checksum(0), senderNode(0), timestamp(0), senderThread(0), topic(0), ttl(0), userDataLen(0){
    userData8[0] = 0;
    if(buffer.size() < 1023)
        return;

    checksum = qFromBigEndian(*((quint16*)(buffer.constData() + 0)));
    senderNode = qFromBigEndian(*((quint32*)(buffer.constData() + 2)));
    timestamp = qFromBigEndian(*((quint64*)(buffer.constData() + 6)));
    senderThread = qFromBigEndian(*((quint32*)(buffer.constData() + 14)));
    topic = qFromBigEndian(*((quint32*)(buffer.constData() + 18)));
    ttl = qFromBigEndian(*((quint16*)(buffer.constData() + 22)));
    userDataLen = qFromBigEndian(*((quint16*)(buffer.constData() + 24)));
    memcpy(userData8, buffer.constData() + 26, userDataLen);
    userData8[userDataLen] = 0x00;
}

Telemetry1::Telemetry1(const Payload payload){

    if(payload.userDataLen != sizeof(Telemetry1) || payload.topic != Telemetry1Type){
        return;
    }

    ch[0] = payload.userData8[0];
    ch[1] = payload.userData8[1];
}

Telemetry2::Telemetry2(const Payload payload){

    if(payload.userDataLen != sizeof(Telemetry2) || payload.topic != Telemetry2Type){
        return;
    }

    a=payload.userData32[0];
    b=payload.userData32[1];
    data[0]=payload.userDataFloat[2];
    data[1]=payload.userDataFloat[3];

}
/*Telecommand::Telecommand(const Payload payload) {

    if(payload.userDataLen != sizeof(Telecommand) || payload.topic != TelecommandType){
        return;
    }

}*/

