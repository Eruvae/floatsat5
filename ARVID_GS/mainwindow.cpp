
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlayer>
#include "tcwindow.h"
#include "powerdata.h"
#include "qmeter.h"
#include "chartview.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>
#include <QtCore/QDebug>

#define LIMIT(x,min,max)   ((x)<(min)?(min):(x)>(max)?(max):(x))

float voltagema, currentma,voltagemb, currentmb,voltagemc, currentmc,voltagemd, currentmd;
bool valve1=false, valve2=false, valve3=false;
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






//    QMediaPlayer *player = new QMediaPlayer(this);
//    player->setMedia(QUrl("D:/Interstellar Main Theme - Extra Extended - Soundtrack by Hans Zimmer (mp3cut.net).mp3"));
//    player->setVolume(100);
//    player->play();
//    qDebug()<< player->errorString();





    link = new SatelliteLink(this);
    // link = new SatelliteLink(this, QHostAddress ("192.168.137.1"), QHostAddress("192.168.137.147"), 5000); // for hotspot wifi
//    link->addTopic(Telemetry1Type);
//    link->addTopic(Telemetry2Type);
    link->addTopic(PowerTelemetryType);
    link->addTopic(FilteredPoseType);
    link->addTopic(IMUDataType);
    link->addTopic(ReactionWheelSpeedType);
    link->addTopic(ActuatorDataType);
    link->addTopic(IRSensorDataType);
    link->addTopic(StarTrackerDataType);
    link->addTopic(OTDataType);
    link->addTopic(RadioPoseDataType);

    link->addTopic(DebugMsgType);
    //gsLink=new SatelliteLink(this, QHostAddress ("192.168.0.109"), QHostAddress("192.168.0.120"), 5000); //GSLink Connection

    //SetupRWRWSpeed();

    ui->speedmeter->setMaxValue(8000);
    ui->speedmeter->setMinValue(-8000);
    ui->speedmeter->setThresholdEnabled(0);
    ui->speedmeter->setForeground(QColor(255, 255, 255));
    ui->speedmeter->setBackground(QColor(0, 0, 0));

    //Setup Traj plot
    ui->trackPlot->setBackground(Qt::black);
    ui->trackPlot->yAxis->setTickLabelColor(Qt::white);
    ui->trackPlot->xAxis->setTickLabelColor(Qt::white);

    //Setup Polar chart
       const qreal angularMin = 0;
       const qreal angularMax = 360;

       const qreal radialMin = 0;
       const qreal radialMax = 140;

       QScatterSeries *series1 = new QScatterSeries();
        series1->setColor(QColor(0, 153, 51));
        series1->setBorderColor(QColor(0, 153, 51));
        series1->setMarkerSize(11);
        series1->append(80, 90);
        series1->clear();


       QPolarChart *chart = new QPolarChart();
       chart->addSeries(series1);
       chart->setBackgroundVisible(false);


       QValueAxis *angularAxis = new QValueAxis();
       angularAxis->setTickCount(5); // First and last ticks are co-located on 0/360 angle.
       angularAxis->setLabelsColor(Qt::white);
       angularAxis->setLabelFormat("%.1f");
       angularAxis->setShadesVisible(false);
       angularAxis->setShadesBrush(QBrush(QColor(0, 0, 0)));
       chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

       QValueAxis *radialAxis = new QValueAxis();
       radialAxis->setTickCount(8);
       radialAxis->setLabelFormat("%d");
       radialAxis->setLabelsColor(Qt::white);
       chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

          series1->attachAxis(radialAxis);
          series1->attachAxis(angularAxis);

       radialAxis->setRange(radialMin, radialMax);
       angularAxis->setRange(angularMin, angularMax);

       ui->pchart->setChart(chart);
       ui->pchart->setRenderHint(QPainter::Antialiasing);

       //End setup polar chart

    SetupGraphCurrent();
    SetupPlotTracking();
    QTimer *timer = new QTimer(this);
    timer->start(100);

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
        int scale=LIMIT(((data.voltage*0.004)-12.2)*100,0,100);
        batteryStateQueue.push_back(scale);
        if (batteryStateQueue.size() > 1000)
            batteryStateQueue.pop_front();

        int batteryStateMean = 0;
        for (int sc : batteryStateQueue)
            batteryStateMean += sc;

        batteryStateMean /= batteryStateQueue.size();

        ui->batterybar->setValue(batteryStateMean);
        ui->battsoc_txt->setText(QString::number(batteryStateMean)+"%");

        //Battery bar colors
          QString value1 = "QProgressBar::chunk {background: rgb(221, 47, 4)};";
          QString value2 = "QProgressBar::chunk {background: rgb(226, 131, 29)};";
          QString value3 = "QProgressBar::chunk {background: rgb(118, 255, 0)};";
          if(ui->batterybar->value()<30)
              ui->batterybar->setStyleSheet(value1);
          if(ui->batterybar->value()>30 && ui->batterybar->value()<70)
              ui->batterybar->setStyleSheet(value2);
          if(ui->batterybar->value()>=70)
              ui->batterybar->setStyleSheet(value3);

        voltagema=data.mota_voltage*0.004;
        currentma=data.mota_current*0.2;
        voltagemb=data.motb_voltage*0.004;
        currentmb=data.motb_current*0.2;
        currentmc=data.motc_current*0.2;
        voltagemc=data.motc_voltage*0.004;
        currentmd=data.motd_current*0.2;
        voltagemd=data.motd_voltage*0.004;

        emit powerDataUpdate();
        //qDebug() << "Graph Value = "<< graphvaluecurrent << endl;

        //gsLink->write<PowerTelemetry>(PowerTelemetryType, data);
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
        ui->lcddyaw->display(data.dyaw);
        ui->lcddpitch->display(data.dpitch);
        ui->lcddroll->display(data.droll);

        float valuex=data.x, valuey=data.y;

        double key = QTime::currentTime().msecsSinceStartOfDay()/1000.0; // time elapsed since start of demo, in seconds
        static double lastPointKey = 0;
        if (key-lastPointKey > 0.5)
        {
            ui->trackPlot->graph(0)->addData(-valuey,valuex);
        }

        lastPointKey = key;

        ui->trackPlot->graph(0)->removeDataBefore(lastPointKey);
//      ui->trackPlot->graph(0)->data()->clear();
//      ui->trackPlot->replot();


        break;
    }

    case IMUDataType:
    {
        ui->lcdGx->display(payload.imuData.gyro_x*2000.0/INT16_MAX);
        ui->lcdGy->display(payload.imuData.gyro_y*2000.0/INT16_MAX);
        ui->lcdGz->display(payload.imuData.gyro_z*2000.0/INT16_MAX);
        ui->lcdAx->display(payload.imuData.acc_x*2.0/INT16_MAX);
        ui->lcdAy->display(payload.imuData.acc_y*2.0/INT16_MAX);
        ui->lcdAz->display(payload.imuData.acc_z*2.0/INT16_MAX);
        ui->lcdMx->display(payload.imuData.mag_x*2.0/INT16_MAX);
        ui->lcdMy->display(payload.imuData.mag_y*2.0/INT16_MAX);
        ui->lcdMz->display(payload.imuData.mag_z*2.0/INT16_MAX);
        ui->lcdTemp->display(payload.imuData.temp*0.125);

        break;
    }

    case ReactionWheelSpeedType:
    {
        ui->lcdRWSpeed->display(payload.reactionWheelSpeed);

        ui->speedmeter->setValue(payload.reactionWheelSpeed);


        break;
    }

    case ActuatorDataType:
    {
        ActuatorData data(payload);
        valve1=(data.valveStatus&0b1)==0b1;
        valve2=(data.valveStatus&0b10)==0b10;
        valve3=(data.valveStatus&0b100)==0b100;

        //ui->test->display(data.dutyCycle);
        break;
    }
    case IRSensorDataType:
    {
        IRSensorData data(payload);
        ui->lcdrange1->display(data.range1);
        ui->lcdrange2->display(data.range2);
        ui->lcddistance->display(data.distance);
        ui->lcdangle->display(data.angle*180/M_PI);
        break;

    }

    case StarTrackerDataType:
    {
        Pose2D data = payload.starTrackerPose;
        ui->starLcdx->display(data.x);
        ui->starLcdy->display(data.y);
        ui->starLcdAngle->display(data.yaw);


        break;
    }

    case OTDataType:
    {
        OTData data(payload);

        break;
    }

    case RadioPoseDataType:
    {
        RadioPoseData data(payload);
        ui->rpLcdx->display(data.x);
        ui->rpLcdy->display(data.y);

        break;
    }

    case DebugMsgType:
    {
        qDebug("%s", payload.userData8);
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

void MainWindow::on_actionManual_Control_triggered()
{
    manualControl = new ManualControl(this, link);
    manualControl->show();
}


void MainWindow::on_pushButton_clicked()
{
    PowerData powerdata(this, link);
    powerdata.setModal(true);
    powerdata.exec();

}

void MainWindow::on_actionSupport_and_Maintainance_triggered()
{
    support = new Support(this, link);
    support->show();
}
