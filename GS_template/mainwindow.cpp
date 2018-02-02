#include "mainwindow.h"
#include "ui_mainwindow.h"

int counter=0;
int missedPackets;
bool graphvalue=0;
int MissedPackets=0;
int recievedPackets=0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mcWindow(0)
{
    ui->setupUi(this);


    link = new SatelliteLink(this);
    //link->addTopic(Telemetry1Type);
    //link->addTopic(Telemetry2Type);
    //link->addTopic(PowerTelemetryType);

    //link2 = new SatelliteLink(this, QHostAddress("192.168.0.120"), QHostAddress("192.168.0.109"), 5000);
    //link2->addTopic(Telemetry1Type);
    //link2->addTopic(Telemetry2Type);
    //link2->addTopic(PowerTelemetryType);

    setupGraph();
    QTimer *timer = new QTimer(this);
    timer->start(100);
    connect(this, SIGNAL(PacketSignal()), this, SLOT(setupGraphrealtimeDataSlot()));
    //connect(link, SIGNAL(readReady()), this, SLOT(readFromLink()));
    connect(ui->pb,SIGNAL(clicked()),this,SLOT(sendtelecommand()));
    connect(ui->saveGraph,SIGNAL(clicked()),this,SLOT(on_pushButton_clicked()));
    connect(ui->comboTC, SIGNAL(currentIndexChanged(int)), ui->stackedTCData, SLOT(setCurrentIndex(int)));

    //connect(link2, SIGNAL(readReady()), this, SLOT(readFromGSLink()));
}

void MainWindow::readFromGSLink()
{
    Payload payload = link2->read();
    //qDebug() << "Topic ID received: " << payload.topic << endl;
    //qDebug()<<"msg from link";
    switch(payload.topic){
        case  Telemetry1Type:{
             Telemetry1 t1 = payload.telemetry1;
             //qDebug()<<"I recieved Telemetry 1 "<<t1.ch[0]<<t1.ch[1]<<endl;
             ui->telemetry1->setText(QString::number(t1.ch[0]));
             ui->telemetry1->setText(QString("Telemetry 1 ->, First Character=%1 and Second Character=%2").arg(t1.ch[0]).arg(t1.ch[1])) ;
            break;
        }  //end case Telemetry1Type

    case  Telemetry2Type:{
         Telemetry2 t2 = payload.telemetry2;
         //qDebug()<<"I recieved Telemetry 2"<<t2.a<<t2.b<< t2.data[0 ]<< t2.data[1]<<endl;

         ui->telemetry2->setText(QString("Telemetry 2 -> %1 %2 %3 %4").arg(t2.a).arg(t2.b).arg(t2.data[0 ]).arg(t2.data[1 ])) ;
         missedPackets=counter - t2.a;

         if(missedPackets==0)
             setSignal(Qt::green);
         else
              setSignal(Qt::red);
         counter=t2.a+1;
        break;

    }          // Telemetry2Type
    case PowerTelemetryType:
    {
        //PowerTelemetry data(payload);

        qDebug() << "Power Telemetry received!" << endl;

        ui->lcdVoltage->display(payload.powerData.voltage*0.004);
        ui->lcdCurrent->display((payload.powerData.current*0.32 - 165) / 1000);
        break;
    }


    default:
        break;
    return;
    }  // end switch
}

void MainWindow::readFromLink(){
    Payload payload = link->read();
    //qDebug() << "Topic ID received: " << payload.topic << endl;
    //qDebug()<<"msg from link";
    switch(payload.topic){
        case  Telemetry1Type:{
             Telemetry1 t1 = payload.telemetry1;
             //qDebug()<<"I recieved Telemetry 1 "<<t1.ch[0]<<t1.ch[1]<<endl;
             ui->telemetry1->setText(QString::number(t1.ch[0]));
             ui->telemetry1->setText(QString("Telemetry 1 ->, First Character=%1 and Second Character=%2").arg(t1.ch[0]).arg(t1.ch[1])) ;

             link2->write<Telemetry1>(payload.topic, payload.telemetry1);

            break;
        }  //end case Telemetry1Type




    case  Telemetry2Type:{
         Telemetry2 t2 = payload.telemetry2;
         //qDebug()<<"I recieved Telemetry 2"<<t2.a<<t2.b<< t2.data[0 ]<< t2.data[1]<<endl;

         ui->telemetry2->setText(QString("Telemetry 2 -> %1 %2 %3 %4").arg(t2.a).arg(t2.b).arg(t2.data[0 ]).arg(t2.data[1 ])) ;
         missedPackets=counter - t2.a;

         if(missedPackets==0)
             setSignal(Qt::green);
         else
              setSignal(Qt::red);
         counter=t2.a+1;

         link2->write<Telemetry2>(payload.topic, payload.telemetry2);

        break;

    }          // Telemetry2Type
    case PowerTelemetryType:
    {
        //PowerTelemetry data(payload);

        qDebug() << "Power Telemetry received!" << endl;

        ui->lcdVoltage->display(payload.powerData.voltage*0.004);
        ui->lcdCurrent->display((payload.powerData.current*0.32 - 165) / 1000);

        link2->write<PowerTelemetry>(payload.topic, payload.powerData);

        break;
    }


    default:
        break;
    return;
    }  // end switch

}
//lineEdit_P_2->text().toFloat();
Telecommand sendme;
void MainWindow::sendtelecommand()
{
   qDebug()<<"we are in sendTelecommand";
   float data; char id;
   //sendme.data=ui->data->text().toFloat();
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
   }
   int written = link->write<Telecommand>(TelecommandType,sendme);
   qDebug() << "Bytes written: " << written << endl;

}

void MainWindow::setSignal(QColor color)
{
    if(color==Qt::green){
    ui->graph_temp->graph(1)->setPen(QPen(Qt::green));
    recievedPackets++;
    ui->RecievedPackets->setText(QString("%1").arg(recievedPackets));
    }

    else{
    ui->graph_temp->graph(1)->setPen(QPen(Qt::red));
    MissedPackets++;
    ui->missedPackets->setText(QString("%1").arg(MissedPackets));
    }
    emit PacketSignal();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbManualControl_clicked()
{
    if (mcWindow == 0)
    {
        mcWindow = new ManualControl(this, link);
    }
    mcWindow->exec();
}
