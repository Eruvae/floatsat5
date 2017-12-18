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

//Telemetry1::Telemetry1(const Payload payload){

//    if(payload.userDataLen != sizeof(Telemetry1) || payload.topic != Telemetry1Type){
//        return;
//    }

//    ch[0] = payload.userData8[0];
//    ch[1] = payload.userData8[1];
//}

//Telemetry2::Telemetry2(const Payload payload){

//    if(payload.userDataLen != sizeof(Telemetry2) || payload.topic != Telemetry2Type){
//        return;
//    }

//    a=payload.userData32[0];
//    b=payload.userData32[1];
//    data[0]=payload.userDataFloat[2];
//    data[1]=payload.userDataFloat[3];

//}

PowerTelemetry::PowerTelemetry(const Payload payload)
{
    voltage = payload.userData16[0];
    current = payload.userData16[1];
    mota_voltage=payload.userData16[2];
    mota_current=payload.userData16[3];
    motb_voltage=payload.userData16[4];
    motb_current=payload.userData16[5];
    motc_voltage=payload.userData16[6];
    motc_current=payload.userData16[7];
    motd_voltage=payload.userData16[8];
    motd_current=payload.userData16[9];
}

FilteredPose::FilteredPose(const Payload payload)
{
    x=payload.userDataFloat[0];
    y=payload.userDataFloat[1];
    z=payload.userDataFloat[2];
    yaw=payload.userDataFloat[3];
    pitch=payload.userDataFloat[4];
    roll=payload.userDataFloat[5];
    dyaw=payload.userDataFloat[6];
    dpitch=payload.userDataFloat[7];
    droll=payload.userDataFloat[8];
}

//IMUData::IMUData(const Payload payload)
//{
//    memcpy(this->gyro, payload.imuData.gyro, 3*sizeof(int16_t));
//    memcpy(this->acc, payload.imuData.acc, 3*sizeof(int16_t));
//    memcpy(this->acc, payload.imuData., 3*sizeof(int16_t));


//}




/*Telecommand::Telecommand(const Payload payload) {

    if(payload.userDataLen != sizeof(Telecommand) || payload.topic != TelecommandType){
        return;
    }

}*/

