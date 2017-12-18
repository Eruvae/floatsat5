#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <QtGlobal>
#include "basics.h"

#define USER_DATA_MAX_LEN 992


enum PayloadType{

    //Telemetry Type
    PowerTelemetryType=5000,
    FilteredPoseType=5001,
    IMUDataType=5002,
    ReactionWheelSpeedType=5003,
    IRSensorDataType=5004,

    //Telecommand Type
    TelecommandType=100

};

struct __attribute__((packed)) IMUData
{
    int16_t gyro_x, gyro_y, gyro_z;
    int16_t acc_x, acc_y, acc_z;
    int16_t mag_x, mag_y, mag_z;
    int16_t temp;

};

struct __attribute__((packed)) Payload
{
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
        IMUData imuData;
        int16_t reactionWheelSpeed;

    };
    Payload();
    Payload(const QByteArray &buffer);
};


struct __attribute__((packed))PowerTelemetry
{
    int16_t voltage;
    int16_t current;
    int16_t mota_voltage;
    int16_t mota_current;
    int16_t motb_voltage;
    int16_t motb_current;
    int16_t motc_voltage;
    int16_t motc_current;
    int16_t motd_voltage;
    int16_t motd_current;

    PowerTelemetry(const Payload payload);
};

struct __attribute__((packed))FilteredPose
{
    float x, y, z;
    float yaw, pitch, roll;
    float dyaw, dpitch, droll;
    FilteredPose(const Payload payload);

};




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
};

struct __attribute__((packed)) Telecommand
{
    uint8_t id;
    TCdata data;
};




#endif // PAYLOAD_H

