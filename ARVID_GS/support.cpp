#include "support.h"
#include "ui_support.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QThread>

#define CALIB_IMU 0x00
#define SEND_POSE 0x01
#define SEND_RW_SPEED 0x02
#define THRUSTER_CONTROL 0x03
#define DEACTIVATE_CONTROLLER 0x04
#define CHANGE_PC_MODE	0x05
#define RPI_COMMAND	0x06
#define SEND_CONTROL_PARAMS 0x07
#define SEND_ROTATION_SPEED 0x08
#define SEND_POSE_TO_LIST 0x09

Support::Support(QWidget *parent, SatelliteLink *link) :
    QDialog(parent),
    ui(new Ui::Support),
    TCLink(link)
{
    ui->setupUi(this);
}

void Support::telecommand()
{
    Telecommand sender;

    if (ui->tabWidget->currentIndex()==0)
    {
        sender.id==CALIB_IMU;

        if (ui->gyrobutton->isChecked())
            sender.data.imu_com=(IMUCommand)0;
        else if (ui->accbutton->isChecked())
            sender.data.imu_com=(IMUCommand)1;
        else if (ui->magbutton->isChecked())
            sender.data.imu_com=(IMUCommand)2;

        TCLink->write<Telecommand>(TelecommandType,sender);

    }

    else if (ui->tabWidget->currentIndex()==1)
    {
        sender.id==SEND_POSE;

        sender.data.pose.x = ui->PoseX->value();
        sender.data.pose.y = ui->PoseY->value();
        sender.data.pose.yaw = ui->PoseYaw->value();

        TCLink->write<Telecommand>(TelecommandType,sender);
    }

    else if (ui->tabWidget->currentIndex()==2)
    {
        sender.id==SEND_RW_SPEED;

        sender.data.wheel_target_speed=ui->RWSpeed_3->value();

        TCLink->write<Telecommand>(TelecommandType,sender);
    }

    else if (ui->tabWidget->currentIndex()==3)
    {
        sender.id==THRUSTER_CONTROL;

        uint8_t data = 0;
        if (ui->T1_3->isChecked()) data |= 0b1;
        if (ui->T2_3->isChecked()) data |= 0b10;
        if (ui->T3_3->isChecked()) data |= 0b100;
        sender.data.valveControl = data;

        TCLink->write<Telecommand>(TelecommandType,sender);
    }

    else if (ui->tabWidget->currentIndex()==4)
    {
        sender.id==SEND_CONTROL_PARAMS;

        sender.data.controlParams.attP = ui->attP_3->value();
        sender.data.controlParams.attD = ui->attD_3->value();
        sender.data.controlParams.attI = ui->attI_3->value();
        sender.data.controlParams.traP = ui->trajP_3->value();
        sender.data.controlParams.traD = ui->trajD_3->value();
        sender.data.controlParams.traI = ui->trajI_3->value();
        sender.data.controlParams.rotP = ui->rotP_3->value();
        sender.data.controlParams.rotD = ui->rotD_3->value();
        sender.data.controlParams.rotI = ui->rotI_3->value();

        TCLink->write<Telecommand>(TelecommandType,sender);
    }

    else
    {
        sender.id==SEND_ROTATION_SPEED;

        sender.data.rotationSpeed= ui->rotationSpeed_3->value();

        TCLink->write<Telecommand>(TelecommandType,sender);
    }


}



Support::~Support()
{
    delete ui;
}

void Support::on_pushButton_clicked()
{
    telecommand();
}
