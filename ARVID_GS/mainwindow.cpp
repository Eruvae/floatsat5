
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlayer>
#include "tcwindow.h"
#include "powerdata.h"
#include "qmeter.h"
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
int MissedPackets=0;
int recievedPackets=0;
float Power;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    link->addTopic(PoseControllerModeType);
    link->addTopic(MissionStateType);


    SetupSpeedMeter();
    SetupGraphCurrent();
    SetupPlotTracking();




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

        if (valuex>0 && -valuey>0)
        {

        double key = QTime::currentTime().msecsSinceStartOfDay()/1000.0; // time elapsed since start of demo, in seconds

        static double lastPointKey = 0;

        if (key-lastPointKey > 3)
          {
            track->addData(-valuey, valuex);
            lastPointKey=key;
           }

        trackline->addData(-valuey, valuex);

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

        static float oldGo = 0, oldgo = 0, oldalpha = 0;
        if (data.found && (oldGo != data.G0 || oldgo != data.g0 || oldalpha != data.alpha))
        {
            float Go=data.G0 , go=data.g0+0.2, alpha=data.alpha;
            oldGo = data.G0; oldgo = data.g0; oldalpha = alpha;

            float r= sqrt( Go*Go + go*go);
            float angle= atan(Go/go) * (180/PI);
            float otYaw = M_PI + alpha + atan2(Go, go);
            float angleGlob = alpha + M_PI - otYaw + ui->lcdyaw->value()*M_PI/180;
            float otX = r * cos(angleGlob);
            float otY = r * sin(angleGlob);
            float otXAbs = (r * cos (angleGlob) + ui->lcdx->value());
            float otYAbs = (r * sin (angleGlob) - ui->lcdy->value());

            ui->lcdNumber->display(otX+ui->lcdx->value());
            ui->lcdNumber_2->display(otY-ui->lcdy->value());
            ui->lcdNumber_3->display(alpha);
            ui->lcdNumber_4->display(data.found);

            ui->trackPlot->graph(0)->addData(otXAbs, otYAbs);
            ui->trackPlot->replot();
            ui->trackPlot->graph(0)->clearData();
        }

        break;

    } //end case OTDataType

    case RadioPoseDataType:
    {
        RadioPoseData data(payload);
        ui->rpLcdx->display(data.x);
        ui->rpLcdy->display(data.y);


        break;

    } //end case RadioPoseDataType
    case PoseControllerModeType:
        if (payload.enumData == 0)
            ui->controlMode->setText("Stand By");
        else if (payload.enumData == 1)
            ui->controlMode->setText("Goto Pose");
        else if (payload.enumData == 2)
            ui->controlMode->setText("Follow Trajectory");
        else if (payload.enumData == 3)
            ui->controlMode->setText("Change Attitude");
        else if (payload.enumData == 4)
            ui->controlMode->setText("Rotate");
        else if (payload.enumData == 5)
            ui->controlMode->setText("Follow Trajectory T");
        break;
    case MissionStateType:
        if (payload.enumData == 0)
            ui->missionState->setText("Stand By");
        else if (payload.enumData == 1)
            ui->missionState->setText("Start Searching");
        else if (payload.enumData == 2)
            ui->missionState->setText("Searching Target");
        else if (payload.enumData == 3)
            ui->missionState->setText("Moving to Target");
        else if (payload.enumData == 4)
            ui->missionState->setText("Rendezvouz Target");
        else if (payload.enumData == 5)
            ui->missionState->setText("Docking");
        break;

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


