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
#define SEND_TRAJECTORY 0x0A

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
        send.id = DEACTIVATE_CONTROLLER;
        send.data.boolData = ui->activateControllerBox_3->isChecked();

        TCLink->write<Telecommand>(TelecommandType,send);
    }


    else if (ui->tabWidget->currentIndex()==1)
    {
        send.id = CHANGE_PC_MODE;
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
        else if (ui->trajectoryTbutton->isChecked())
            send.data.pcMode = 5;

        TCLink->write<Telecommand>(TelecommandType,send);
    }

    else if (ui->tabWidget->currentIndex()==2)
    {
        send.id = RPI_COMMAND;
        if (ui->stbutton->isChecked())
            send.data.rpiComData.command = 0;
        else if (ui->otbutton->isChecked())
            send.data.rpiComData.command = 1;
        else if (ui->rpbutton->isChecked())
            send.data.rpiComData.command = 2;


        send.data.rpiComData.enable = ui->rpicheck->isChecked();


        TCLink->write<Telecommand>(TelecommandType,send);
    }

    else if (ui->tabWidget->currentIndex()==3)
    {
        send.id = SEND_POSE_TO_LIST;
        for (Pose2D pose : poses)
        {
            send.data.pose = pose;
            TCLink->write<Telecommand>(TelecommandType,send);
            QThread::msleep(25);
        }
        return;
    }

    else
    {
        send.id = SEND_TRAJECTORY;
        if (ui->comboLineTraj->isChecked())
        {
            send.data.trajData.type = LINE;
            send.data.trajData.startTime = ui->lStartT->value() * SECONDS;
            send.data.trajData.endTime = ui->lEndT->value() * SECONDS;
            send.data.trajData.lineData.startPose = {ui->startPoseX->value(), ui->startPoseY->value(), ui->startPoseYaw->value()};
            send.data.trajData.lineData.endPose = {ui->endPoseX->value(), ui->endPoseY->value(), ui->endPoseYaw->value()};
        }
        else if (ui->comboCircleTraj->isChecked())
        {
            send.data.trajData.type = CIRCLE;
            send.data.trajData.startTime = ui->cStartT->value() * SECONDS;
            send.data.trajData.endTime = ui->cEndT->value() * SECONDS;
            send.data.trajData.circleData.r = ui->cR->value();
            send.data.trajData.circleData.centerPose = {ui->cX->value(), ui->cY->value(), 0.f};
            send.data.trajData.circleData.betaStart = ui->cAngleS->value();
            send.data.trajData.circleData.betaEnd = ui->cAngleE->value();
        }
        TCLink->write<Telecommand>(TelecommandType,send);
    }

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

}
