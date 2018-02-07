
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlayer>
#include "tcwindow.h"
#include "powerdata.h"
#include "qmeter.h"
#include "chartview.h"
#include "qcustomplot.h"
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
#define PI 3.14159265

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


    SetupSpeedMeter();
    SetupGraphCurrent();
    SetupPlotTracking();
    SetupRadar();



    QTimer *timer = new QTimer(this);
    timer->start(100);

    connect(link, SIGNAL(readReady()), this, SLOT(readFromLink()));

}

void MainWindow::readFromLink()
{
    Payload payload = link->read();

    switch(payload.topic)

{

    case PowerTelemetryType:
    {
        PowerTelemetry data(payload);

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

        break;

    } //end case PowerTelemetryType

    case FilteredPoseType:
    {
        FilteredPose data(payload);

        //float oldvaluex=0, oldvaluey=0;

        ui->lcdx->display(data.x);
        ui->lcdy->display(data.y);
        ui->lcdz->display(data.z);
        ui->lcdyaw->display(data.yaw);
        ui->lcdpitch->display(data.pitch);
        ui->lcdroll->display(data.roll);
        ui->lcddyaw->display(data.dyaw);
        ui->lcddpitch->display(data.dpitch);
        ui->lcddroll->display(data.droll);

        //QCPItemLine *lend = new QCPItemLine(ui->trackPlot);
        float valuex=data.x, valuey=data.y;
        //lend->start->setCoords(oldvaluex,oldvaluey);
        //lend->end->setCoords(-valuey,valuex);
        //ui->trackPlot->graph(0)->addData(-valuey,valuex);

        QCPCurve *track = new QCPCurve(ui->trackPlot->xAxis, ui->trackPlot->yAxis);


        //oldvaluex=-valuey, oldvaluey=valuex;

        if (valuex>0 && -valuey>0)
        {

        double key = QTime::currentTime().msecsSinceStartOfDay()/1000.0; // time elapsed since start of demo, in seconds

        track->setPen(QPen(Qt::blue));

        static double lastPointKey = 0;

        if (key-lastPointKey > 3)
          {

            track->addData(key, -valuey, valuex);
//            ui->trackPlot->graph(0)->addData(-valuey, valuex);
            lastPointKey=key;
           }

//         ui->trackPlot->graph(1)->addData(-valuey, valuex);

        ui->trackPlot->replot();
        }


        break;

    } //end case FilteredPoseType

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

    } //end case IMUDataType

    case ReactionWheelSpeedType:
    {
        ui->lcdRWSpeed->display(payload.reactionWheelSpeed);
        ui->speedmeter->setValue(payload.reactionWheelSpeed);

        break;

    } //end case ReactionWheelSpeedType

    case ActuatorDataType:
    {
        ActuatorData data(payload);
        valve1=(data.valveStatus&0b1)==0b1;
        valve2=(data.valveStatus&0b10)==0b10;
        valve3=(data.valveStatus&0b100)==0b100;

        break;

    } //end case ActuatorDataType

    case IRSensorDataType:
    {
        IRSensorData data(payload);
        ui->lcdrange1->display(data.range1);
        ui->lcdrange2->display(data.range2);
        ui->lcddistance->display(data.distance);
        ui->lcdangle->display(data.angle*180/M_PI);

        break;

    } //end case IRSensorDataType

    case StarTrackerDataType:
    {
        Pose2D data = payload.starTrackerPose;
        ui->starLcdx->display(data.x);
        ui->starLcdy->display(data.y);
        ui->starLcdAngle->display(data.yaw);

        break;

    } //end case StarTrackerDataType

    case OTDataType:
    {
        OTData data(payload); 

        float Go=data.G0 , go=data.g0+0.2, alpha=data.alpha;

        float r= sqrt( Go*Go + go*go);
        float angle= atan(Go/go) * (180/PI);

        if (angle<0)
        {
            double key = QTime::currentTime().msecsSinceStartOfDay()/1000.0; // time elapsed since start of demo, in seconds
            static double lastPointKey = 0;

            if (key-lastPointKey > 0.01)
            {
                series1->append(-angle, r); //to plot
                chart->addSeries(series1); //to plot
            }

            lastPointKey=key;

        }

        else
        {
            double key = QTime::currentTime().msecsSinceStartOfDay()/1000.0; // time elapsed since start of demo, in seconds
            static double lastPointKey = 0;

        if (key-lastPointKey > 0.01)
        {
            series1->append(angle, r); //to plot
            chart->addSeries(series1); //to plot
        }

            lastPointKey=key;
        }

        series1->clear(); //to plot

        
        break;

    } //end case OTDataType

    case RadioPoseDataType:
    {
        RadioPoseData data(payload);
        ui->rpLcdx->display(data.x);
        ui->rpLcdy->display(data.y);


        break;

    } //end case RadioPoseDataType

    case DebugMsgType:
    {
        qDebug("%s", payload.userData8);
        break;

    } //end case DebugMsgType

    default:

        break;

    return;

}  // end switch

}


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

void MainWindow::on_actionSupport_and_Maintainance_triggered()
{
    support = new Support(this, link);
    support->show();
}


void MainWindow::on_pushButton_clicked()
{
    PowerData powerdata(this, link);
    powerdata.setModal(true);
    powerdata.exec();

}


