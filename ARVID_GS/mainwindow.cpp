
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlayer>
#include "tcwindow.h"

int counter=0;
int missedPackets;
//double graphvaluecurrent=0;
int MissedPackets=0;
int recievedPackets=0;
float Power;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    QMediaPlayer *player = new QMediaPlayer(this);
    player->setMedia(QUrl("D:/Interstellar Main Theme - Extra Extended - Soundtrack by Hans Zimmer (mp3cut.net).mp3"));
    player->setVolume(100);
    player->play();
    qDebug()<< player->errorString();






    link = new SatelliteLink(this);
//    link->addTopic(Telemetry1Type);
//    link->addTopic(Telemetry2Type);
    link->addTopic(PowerTelemetryType);
    link->addTopic(FilteredPoseType);
    gsLink=new SatelliteLink(this, QHostAddress ("192.168.0.109"), QHostAddress("192.168.0.120"), 5000); //GSLink Connection

    //SetupRWSpeedMeter();


    SetupGraphCurrent();
    QTimer *timer = new QTimer(this);
    timer->start(100);

    SetupCompass();


    //connect(this, SIGNAL(PacketSignal(double)), this, SLOT(SetupRealtimeDataSlotCurrent(double)));
    connect(link, SIGNAL(readReady()), this, SLOT(readFromLink()));
    //connect(ui->pb,SIGNAL(clicked()),this,SLOT(sendtelecommand()));
    //connect(ui->saveGraph,SIGNAL(clicked()),this,SLOT(on_pushButton_clicked()));
    //connect(ui->comboTC, SIGNAL(currentIndexChanged(int)), ui->stackedTCData, SLOT(setCurrentIndex(int)));
}

void MainWindow::readFromLink(){
    Payload payload = link->read();
    //qDebug() << "Topic ID received: " << payload.topic << endl;
    //qDebug()<<"msg from link";
    switch(payload.topic){
//       case  Telemetry1Type:{
//             Telemetry1 t1(payload);
//             //qDebug()<<"I recieved Telemetry 1 "<<t1.ch[0]<<t1.ch[1]<<endl;
//             //ui->telemetry1->setText(QString::number(t1.ch[0]));
//             //ui->telemetry1->setText(QString("Telemetry 1 ->, First Character=%1 and Second Character=%2").arg(t1.ch[0]).arg(t1.ch[1])) ;
//             break;

//        }  //end case Telemetry1Type




//    case  Telemetry2Type:{
//         Telemetry2 t2(payload);
//         //qDebug()<<"I recieved Telemetry 2"<<t2.a<<t2.b<< t2.data[0 ]<< t2.data[1]<<endl;

//        //ui->telemetry2->setText(QString("Telemetry 2 -> %1 %2 %3 %4").arg(t2.a).arg(t2.b).arg(t2.data[0 ]).arg(t2.data[1 ])) ;
//         missedPackets=counter - t2.a;

//         if(missedPackets==0)
//             setSignal(Qt::green);
//         else
//              setSignal(Qt::red);
//         counter=t2.a+1;
//        break;

//    }          // Telemetry2Type

    case PowerTelemetryType:
    {
        PowerTelemetry data(payload);

        //qDebug() << "Power Telemetry received!" << endl;

        ui->lcdVoltage->display(data.voltage*0.004);
        ui->lcdCurrent->display(data.current*0.32 - 165);
        Power=(data.voltage*0.004)*((data.current*0.32-165)/1000);
        ui->lcdPower->display(Power);
        double graphvaluecurrent=(data.current*0.32-165)/1000;
        SetupRealtimeDataSlotCurrent(graphvaluecurrent);
        //qDebug() << "Graph Value = "<< graphvaluecurrent << endl;

        gsLink->write<PowerTelemetry>(PowerTelemetryType, data);
        break;
    } //end case PowerTelemetryType!

    case FilteredPoseType:
    {
        FilteredPose data(payload);

        ui->lcdx->display(data.x);
        ui->lcdy->display(data.y);
        ui->lcdz->display(data.z);
        ui->lcdyaw->display(data.yaw);
        ui->lcdpitch->display(data.pitch);
        ui->lcdroll->display(data.roll);
        break;
    }

    case IMUDataType:
    {
        ui->lcdGx->display(payload.imuData.gyro[0]*2000.0/INT16_MAX);
        ui->lcdGy->display(payload.imuData.gyro[1]*2000.0/INT16_MAX);
        ui->lcdGz->display(payload.imuData.gyro[2]*2000.0/INT16_MAX);
        ui->lcdAx->display(payload.imuData.acc[0]*2.0/INT16_MAX);
        ui->lcdAy->display(payload.imuData.acc[1]*2.0/INT16_MAX);
        ui->lcdAz->display(payload.imuData.acc[2]*2.0/INT16_MAX);
        ui->lcdMx->display(payload.imuData.mag[0]*2.0/INT16_MAX);
        ui->lcdMx->display(payload.imuData.mag[1]*2.0/INT16_MAX);
        ui->lcdMx->display(payload.imuData.mag[2]*2.0/INT16_MAX);
        ui->lcdTemp->display(payload.imuData.temp*0.125);
        break;
    }

    case ReactionWheelSpeedType:
    {
        ui->lcdRWSpeed->display(payload.reactionWheelSpeed);
        break;
    }


    default:
        break;
    return;

}  // end switch
}

//lineEdit_P_2->text().toFloat();
//Telecommand sendme;
void MainWindow::sendtelecommand()
{
   qDebug()<<"we are in sendTelecommand";
   //float data; char id;
   //sendme.data=ui->data->text().toFloat();
   //sendme.id = ui->comboTC->currentIndex();

   //qDebug() << "Current index: " << ui->comboTC->currentIndex() << endl;

   //switch(ui->comboTC->currentIndex())
   //{
//   case 0: // CALIB_IMU
//       sendme.data.imu_com = (IMUCommand)ui->comboIMUcal->currentIndex();
//break;
//   case 1: // SEND_POS
//       sendme.data.pose.x = ui->posX->value();
//       sendme.data.pose.y = ui->posY->value();
//       sendme.data.pose.z = ui->posZ->value();
//       sendme.data.pose.pitch = ui->posPitch->value();
//       sendme.data.pose.yaw = ui->posYaw->value();
//       sendme.data.pose.roll = ui->posRoll->value();
    //   break;
  //}

   //int written = link->write<Telecommand>(TelecommandType,sendme);
   //qDebug() << "Bytes written: " << written << endl;

}



//void MainWindow::setSignal(QColor color)
//{
//    if(color==Qt::green){
//    //ui->graph_temp->graph(1)->setPen(QPen(Qt::green));
//    recievedPackets++;
//    //ui->RecievedPackets->setText(QString("%1").arg(recievedPackets));
//    }

//    else{
//    //ui->graph_temp->graph(1)->setPen(QPen(Qt::red));
//    MissedPackets++;
//    //ui->missedPackets->setText(QString("%1").arg(MissedPackets));
//    }
//    //emit PacketSignal();
//}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionTelecommand_Interface_triggered()
{
    tcwindow = new TCWindow(this, link);
    tcwindow->show();

}
