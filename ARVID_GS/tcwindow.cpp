#include "tcwindow.h"
#include "ui_tcwindow.h"

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

TCWindow::TCWindow(QWidget *parent, SatelliteLink *link) :
    QDialog(parent),
    ui(new Ui::TCWindow),
    TCLink(link)
{
    ui->setupUi(this);

}



void TCWindow::telecommandsend()
{
    Telecommand send;

    if (ui->tabWidget->currentIndex()==0)
    {
        send.id == DEACTIVATE_CONTROLLER;
        send.data.boolData = ui->activateControllerBox_3->isChecked();

        TCLink->write<Telecommand>(TelecommandType,send);
    }


    else if (ui->tabWidget->currentIndex()==1)
    {
        if (ui->standbybutton->isChecked())
            send.data.pcMode = 0;
        else if (ui->holdposebutton->isChecked())
            send.data.pcMode = 1;
        else if (ui->trajectorybutton->isChecked())
            send.data.pcMode = 2;
        else if (ui->attitudebutton->isChecked())
            send.data.pcMode = 3;
        else if (ui->rotatebutton->isChecked())
            send.data.pcMode = 4;

        TCLink->write<Telecommand>(TelecommandType,send);
    }

    else if (ui->tabWidget->currentIndex()==2)
    {
        if (ui->stbutton->isChecked())
            send.data.rpiComData.command = 0;
        else if (ui->otbutton->isChecked())
            send.data.rpiComData.command = 1;
        else if (ui->rpbutton->isChecked())
            send.data.rpiComData.command = 2;


        send.data.rpiComData.enable = ui->rpicheck->isChecked();


        TCLink->write<Telecommand>(TelecommandType,send);
    }

    else
    {
        send.id == SEND_POSE_TO_LIST;
        for (Pose2D pose : poses)
        {
            send.data.pose = pose;
            TCLink->write<Telecommand>(TelecommandType,send);
            QThread::msleep(25);
        }
        return;
    }



//    switch(ui->tabWidget->currentIndex())
//    {
//    case 0: // CALIB_IMU
//        if (ui->gyrobutton->isChecked())
//                send.data.imu_com=(IMUCommand)0;
//        else if (ui->accbutton->isChecked())
//                send.data.imu_com=(IMUCommand)1;
//        else if (ui->magbutton->isChecked())
//                send.data.imu_com=(IMUCommand)2;

//        //send.data.imu_com = (IMUCommand)ui->comboIMUcal->currentIndex();
//        break;

//    case 1: // SEND_POS
//        send.data.pose.x = ui->PoseX->value();
//        send.data.pose.y = ui->PoseY->value();
//        send.data.pose.yaw = ui->PoseYaw->value();
//        break;

//    case 2: // SEND_RW_SPEED
//        send.data.wheel_target_speed=ui->RWSpeed_3->value();
//        break;

//    case 3: //THRUSTER_CONTROL
//    {
//        uint8_t data = 0;
//        if (ui->T1_3->isChecked()) data |= 0b1;
//        if (ui->T2_3->isChecked()) data |= 0b10;
//        if (ui->T3_3->isChecked()) data |= 0b100;
//        send.data.valveControl = data;
//        break;
//    }

//    case 4: //DEACTIVATE_CONTROLLER
//        send.data.boolData = ui->activateControllerBox_3->isChecked();
//        break;

//    case 5: //CHANGE_PC_MODE
//        if (ui->standbybutton->isChecked())
//            send.data.pcMode = 0;
//        else if (ui->holdposebutton->isChecked())
//            send.data.pcMode = 1;
//        else if (ui->trajectorybutton->isChecked())
//            send.data.pcMode = 2;
//        else if (ui->attitudebutton->isChecked())
//            send.data.pcMode = 3;
//        else if (ui->rotatebutton->isChecked())
//            send.data.pcMode = 4;

//        //send.data.pcMode = ui->comboPCmode->currentIndex();
//        break;

//    case 6: //RPI_COMMAND
//        if (ui->stbutton->isChecked())
//            send.data.rpiComData.command = 0;
//        else if (ui->otbutton->isChecked())
//            send.data.rpiComData.command = 1;
//        else if (ui->rpbutton->isChecked())
//            send.data.rpiComData.command = 2;

//        //send.data.rpiComData.command = ui->comboRPIcom->currentIndex();
//        send.data.rpiComData.enable = ui->rpicheck->isChecked();
//        //qDebug() << "Bool: " << send.data.rpiComData.enable << endl;
//        break;

//    case 7: //SEND_CONTROL_DATA
//        send.data.controlParams.attP = ui->attP_3->value();
//        send.data.controlParams.attD = ui->attD_3->value();
//        send.data.controlParams.attI = ui->attI_3->value();
//        send.data.controlParams.traP = ui->trajP_3->value();
//        send.data.controlParams.traD = ui->trajD_3->value();
//        send.data.controlParams.traI = ui->trajI_3->value();
//        send.data.controlParams.rotP = ui->rotP_3->value();
//        send.data.controlParams.rotD = ui->rotD_3->value();
//        send.data.controlParams.rotI = ui->rotI_3->value();
//        break;

//    case 8: //ROTATION_SPEED
//        send.data.rotationSpeed= ui->rotationSpeed_3->value();
//        break;
//    }

//    int written = TCLink->write<Telecommand>(TelecommandType,send);
//    qDebug() << "Bytes written: " << written << endl;

}

void TCWindow::readPosesFromFile(QFile &file, QVector<Pose2D> &poses)
{
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox msgBox;
        msgBox.setText("File " + file.fileName() + " not opened.");
        msgBox.exec();
        return;
    }
    char buf[64];
    while(file.readLine(buf, sizeof(buf)) != -1)
    {
        Pose2D pose;
        char *it = buf, *next;

        pose.x = strtod(it, &next);
        if (it == next || *next != ';')
            continue;
        it = next + 1;

        pose.y = strtod(it, &next);
        if (it == next || *next != ';')
            continue;
        it = next + 1;

        pose.yaw = strtod(it, &next);
        if (it == next)
            continue;

        poses.push_back(pose);
    }
}


TCWindow::~TCWindow()
{
    delete ui;
}


void TCWindow::on_pushButton_4_clicked()
{
    telecommandsend();
}

void TCWindow::on_readPoseFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open pose file"), "/", tr("All Files (*)"));
    QFile file(fileName);
    poses.clear();
    readPosesFromFile(file, poses);

    qDebug() << "Poses found: " << poses.size() << endl;
    ui->posesToSendTable->setRowCount(poses.size() + 1);
    ui->posesToSendTable->verticalHeader()->setVisible(false);
    for (int i = 0; i < poses.size(); i++)
    {
        ui->posesToSendTable->setItem(i, 0, new QTableWidgetItem(QString::number(poses[i].x)));
        ui->posesToSendTable->setItem(i, 1, new QTableWidgetItem(QString::number(poses[i].y)));
        ui->posesToSendTable->setItem(i, 2, new QTableWidgetItem(QString::number(poses[i].yaw)));
        //qDebug() << "Pose: (" << p.x << "," << p.y << "," << p.yaw << ")" << endl;
    }
    // TODO: Display Poses, send telecommand
}
