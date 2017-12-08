#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <QtGlobal>
#include "basics.h"

#define USER_DATA_MAX_LEN 992


enum PayloadType{
    Telemetry1Type=5661,
    Telemetry2Type=5771,
    PowerTelemetryType=6000,
    TelecommandType=300
};

struct Payload{
    quint16 checksum;
    quint32 senderNode;
    quint64 timestamp;
    quint32 senderThread;
    quint32 topic;
    quint16 ttl;
    quint16 userDataLen;
    union{
        quint8 userData8[USER_DATA_MAX_LEN / sizeof(quint8)];
        quint16 userData16[USER_DATA_MAX_LEN / sizeof(quint16)];
        quint32 userData32[USER_DATA_MAX_LEN / sizeof(quint32)];
        quint64 userData64[USER_DATA_MAX_LEN / sizeof(quint64)];
        float userDataFloat[USER_DATA_MAX_LEN / sizeof(float)];
        double userDataDouble[USER_DATA_MAX_LEN / sizeof(double)];
    };
    Payload();
    Payload(const QByteArray &buffer);
};

#pragma pack(push,1)

struct Telemetry1
{
    char ch[2];
    Telemetry1(const Payload payload);
};

struct Telemetry2
{
    quint32 a,b;
    float data[2];
    Telemetry2(const Payload payload);
};

struct PowerTelemetry
{
    int16_t voltage;
    int16_t current;
    PowerTelemetry(const Payload payload);
};

enum IMUCommand
{
    CALIB_GYRO = 0, CALIB_ACC, CALIB_MAG
};

struct Pose
{
    float x, y, z;
    float yaw, pitch, roll;
};

union TCdata
{
    IMUCommand imu_com;
    Pose pose;
};

struct Telecommand
{
    uint8_t id;
    TCdata data;
};

/*
struct Telecommand
{
       float data;
       char id;
        //Telecommand(const Payload payload);
        //Telecommand() {}

};
*/

#pragma pack(pop)

#endif // PAYLOAD_H

