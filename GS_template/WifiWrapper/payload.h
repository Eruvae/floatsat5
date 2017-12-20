#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <QtGlobal>
#include "basics.h"

#define USER_DATA_MAX_LEN 992


enum PayloadType{
    Telemetry1Type=5661,
    Telemetry2Type=5771,

    PowerTelemetryType=5000,
    FilteredPoseType=5001,
    IMUDataType=5002,
    ReactionWheelSpeedType=5003,
    IRSensorDataType=5004,
    ActuatorDataType=5005,

    //Telecommand Type
    TelecommandType=100
};

struct __attribute__((packed)) PowerTelemetry
{
    int16_t voltage;
    int16_t current;
    //PowerTelemetry(const Payload payload);
};

struct __attribute__((packed)) Telemetry1
{
    char ch[2];
    //Telemetry1(const Payload payload);
};

struct __attribute__((packed)) Telemetry2
{
    quint32 a,b;
    float data[2];
    //Telemetry2(const Payload payload);
};

struct __attribute__((packed)) Payload{
    quint16 checksum;
    quint32 senderNode;
    quint64 timestamp;
    quint32 senderThread;
    quint32 topic;
    quint16 ttl;
    quint16 userDataLen;
    union{
        quint8 userData8[USER_DATA_MAX_LEN / sizeof(quint8)];
        /*quint16 userData16[USER_DATA_MAX_LEN / sizeof(quint16)];
        quint32 userData32[USER_DATA_MAX_LEN / sizeof(quint32)];
        quint64 userData64[USER_DATA_MAX_LEN / sizeof(quint64)];
        float userDataFloat[USER_DATA_MAX_LEN / sizeof(float)];
        double userDataDouble[USER_DATA_MAX_LEN / sizeof(double)];*/
        PowerTelemetry powerData;
        Telemetry1 telemetry1;
        Telemetry2 telemetry2;

    };
    Payload();
    Payload(const QByteArray &buffer);
};

//#pragma pack(push,1)

enum IMUCommand
{
    CALIB_GYRO = 0, CALIB_ACC, CALIB_MAG
};

struct __attribute__((packed)) Pose
{
    float x, y, z;
    float yaw, pitch, roll;
};

union __attribute__((packed)) TCdata
{
    IMUCommand imu_com;
    Pose pose;
    int16_t wheel_target_speed;
    uint8_t valveControl;
};

struct __attribute__((packed)) Telecommand
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

//#pragma pack(pop)

#endif // PAYLOAD_H

