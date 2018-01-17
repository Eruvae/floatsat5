#include "tcwindow.h"
#include "ui_tcwindow.h"

TCWindow::TCWindow(QWidget *parent, SatelliteLink *link) :
    QDialog(parent),
    ui(new Ui::TCWindow),
    TCLink(link)
{
    ui->setupUi(this);
    ui->stackedTCData->setCurrentIndex(ui->comboTC->currentIndex());
    connect(ui->comboTC, SIGNAL(currentIndexChanged(int)), ui->stackedTCData, SLOT(setCurrentIndex(int)));
}

void TCWindow::sendtelecommand()
{
    Telecommand sendme;
       qDebug()<<"we are in sendTelecommand";

       sendme.id = ui->comboTC->currentIndex();

       qDebug() << "Current index: " << ui->comboTC->currentIndex() << endl;

       switch(ui->comboTC->currentIndex())
       {
       case 0: // CALIB_IMU
           sendme.data.imu_com = (IMUCommand)ui->comboIMUcal->currentIndex();
           break;
       case 1: // SEND_POS
           sendme.data.pose.x = ui->posX->value();
           sendme.data.pose.y = ui->posY->value();
           sendme.data.pose.z = ui->posZ->value();
           sendme.data.pose.pitch = ui->posPitch->value();
           sendme.data.pose.yaw = ui->posYaw->value();
           sendme.data.pose.roll = ui->posRoll->value();
           break;

       case 2: // SEND_RW_SPEED
           sendme.data.wheel_target_speed=ui->RWSpeed->value();
           break;


       case 3: //THRUSTER_CONTROL
       {
           uint8_t data = 0;
           if (ui->T1->isChecked()) data |= 0b1;
           if (ui->T2->isChecked()) data |= 0b10;
           if (ui->T3->isChecked()) data |= 0b100;
           sendme.data.valveControl = data;
           break;
       }

       case 4:
           sendme.data.boolData = ui->activateControllerBox->isChecked();
           break;

       /*case 4: //ACTIVATE_CONTROLLER
           sendme.data.boolData = ui->activateControllerBox->isChecked();
           break;*/
       }

       int written = TCLink->write<Telecommand>(TelecommandType,sendme);
       qDebug() << "Bytes written: " << written << endl;

}


TCWindow::~TCWindow()
{
    delete ui;
}

void TCWindow::on_pb_clicked()
{
    sendtelecommand();
}
