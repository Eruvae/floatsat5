#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <QtGlobal>
#include "basics.h"

#define USER_DATA_MAX_LEN 992

#define NANOSECONDS    1LL
#define MICROSECONDS   (1000LL * NANOSECONDS)
#define MILLISECONDS   (1000LL * MICROSECONDS)
#define SECONDS        (1000LL * MILLISECONDS)

enum PayloadType{

    //Telemetry Type
    PowerTelemetryType=5000,
    FilteredPoseType=5001,
    IMUDataType=5002,
    ReactionWheelSpeedType=5003,
    IRSensorDataType=5004,
    ActuatorDataType=5005,
    StarTrackerDataType=5006,
    OTDataType=5007,
    RadioPoseDataType=5008,
    PoseControllerModeType=5010,
    RaspiStatusType=5011,
    MissionStateType=5012,

    DebugMsgType = 6000,

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

struct __attribute__((packed)) Pose2D
{
    float x, y, yaw;
    //Pose2D (const Payload Payload);
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
        bool userDataBool[USER_DATA_MAX_LEN/sizeof(bool)];
        quint8 userData8[USER_DATA_MAX_LEN / sizeof(quint8)];
        quint16 userData16[USER_DATA_MAX_LEN / sizeof(quint16)];
        quint32 userData32[USER_DATA_MAX_LEN / sizeof(quint32)];
        quint64 userData64[USER_DATA_MAX_LEN / sizeof(quint64)];
        float userDataFloat[USER_DATA_MAX_LEN / sizeof(float)];
        double userDataDouble[USER_DATA_MAX_LEN / sizeof(double)];
        IMUData imuData;
        int16_t reactionWheelSpeed;
        Pose2D starTrackerPose;
        int enumData;

    };
    Payload();
    Payload(const QByteArray &buffer);
};

struct __attribute__((packed))ActuatorData
{
    uint8_t valveStatus;
    uint8_t rwDirection;
    float dutyCycle;

    ActuatorData(Payload payload);
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

struct __attribute__((packed)) IRSensorData
{
    uint8_t range1;
    uint8_t range2;
    float distance;
    float angle;
    IRSensorData(const Payload Payload);
};

struct __attribute__((packed)) OTData
{
    float alpha, g0, G0;
    bool found;
    OTData (const Payload Payload);
};

struct __attribute__((packed)) RadioPoseData
{
    float x1,y1, x2, y2;
    RadioPoseData(const Payload Payload);
};

enum IMUCommand
{
    CALIB_GYRO = 0, CALIB_ACC, CALIB_MAG
};

enum RaspiCommand
{
    ST, OT, RD
};

enum class Mode
{
    STANDBY, HOLD_POSE, TRACK_OBJECT, GOTO_POSE, DOCKING
};

enum class PoseControllerMode
{
    STANDBY, GOTO_POSE, FOLLOW_TRAJECTORY, CHANGE_ATTITUDE, ROTATE, FOLLOW_TRAJECTORY_T
};

struct __attribute__((packed)) Pose
{
    float x, y, z;
    float yaw, pitch, roll;
};

struct __attribute__((packed)) RaspiCommandData
{
    int command;
    bool enable;
};

struct __attribute__((packed)) ControlParameters
{
    float attP, attD, attI;
    float traP, traD, traI;
    float rotP, rotD, rotI;
};

#define LINE 0
#define CIRCLE 1

struct __attribute__((packed)) TrajectoryLineData
{
    Pose2D startPose;
    Pose2D endPose;
};

struct __attribute__((packed)) TracetoryCircleData
{
    Pose2D centerPose;
    float r, betaStart, betaEnd;
};

struct __attribute__((packed)) TrajectoryPlanData
{
    union
    {
        TrajectoryLineData lineData;
        TracetoryCircleData circleData;
    };
    int64_t startTime;
    int64_t endTime;
    int type;
};

union __attribute__((packed)) TCdata
{
    IMUCommand imu_com;
    Pose2D pose;
    int16_t wheel_target_speed;
    uint8_t valveControl;
    bool boolData;
    int pcMode;
    RaspiCommandData rpiComData;
    ControlParameters controlParams;
    float rotationSpeed;
    TrajectoryPlanData trajData;
};

struct __attribute__((packed)) Telecommand
{
    uint8_t id;
    TCdata data;
};

struct DebugMessage
{
    char str[64];
};

#endif // PAYLOAD_H

