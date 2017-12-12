

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>



//double Square_Wave() {
//   const int half_period=5;
//   double amplitude= -5;
//   int i=0;
//   int k;
//   double x[100];

//   char command;
//   float data;
//   char buffer[50];
//   sprintf(buffer,"Hello\r\n");
//   //for(k=0;k<100;k++)

//    if (!(i++ % half_period))
//    {
//    amplitude =-amplitude;
//  }

//    x[k]=amplitude;
//      printf("%d\t %12.7f\n",k,x[k]);
//    return amplitude;
//}



void MainWindow::SetupGraphCurrent()

{

    ui->graph_temp->addGraph(); // blue line
    ui->graph_temp->graph(0)->setPen(QPen(QColor(40, 110, 255)));
//    ui->graph_temp->addGraph(); // red line
//    ui->graph_temp->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    ui->graph_temp->xAxis->setLabel("Time Elapsed");
    ui->graph_temp->yAxis->setLabel("Current (mA)");
    //ui->graph_temp->setBackground(QColor(224,224,224));
    ui->graph_temp->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->graph_temp->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->graph_temp->xAxis->setAutoTickStep(false);
    ui->graph_temp->xAxis->setTickStep(2);
    ui->graph_temp->axisRect()->setupFullAxesBox();
    ui->graph_temp->yAxis->setRange(0, 0.4);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->graph_temp->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->graph_temp->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->graph_temp->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->graph_temp->yAxis2, SLOT(setRange(QCPRange)));
    QTimer dataTimer;
    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    //connect(&dataTimer, SIGNAL(timeout()), this, SLOT(SetupRealtimeDataSlotCurrent()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible

}


void MainWindow::SetupRealtimeDataSlotCurrent(double newValue)
{
static QTime time(QTime::currentTime());
// calculate two new data points:
double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
static double lastPointKey = 0;
if (key-lastPointKey > 0.01) // at most add point every 10 ms
{
  // add data to lines:
  ui->graph_temp->graph(0)->addData(key, newValue);
  //ui->graph_temp->graph(1)->addData(key, qCos(key)+qrand()/(double)RAND_MAX*0.5*qSin(key/0.4364));
  // rescale value (vertical) axis to fit the current data:
  //ui->graph_temp->graph(0)->rescaleValueAxis();
  ui->graph_temp->graph(0)->rescaleValueAxis(true);
  lastPointKey = key;
}
// make key axis range scroll with the data (at a constant range size of 8):
ui->graph_temp->xAxis->setRange(key, 8, Qt::AlignRight);
ui->graph_temp->replot();

// calculate frames per second:
static double lastFpsKey;
static int frameCount;
++frameCount;
if (key-lastFpsKey > 2) // average fps over 2 seconds
{
  ui->statusBar->showMessage(
        QString("%1 FPS, Total Data points: %2")
        .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
        .arg(ui->graph_temp->graph(0)->data()->size()) , 0);
  lastFpsKey = key;
  frameCount = 0;
}

}


void MainWindow::on_pushButton_clicked()
{
    QString fileName("graph_temp.png");
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << file.errorString();
    } else {
        ui->graph_temp->savePng(fileName);

    }
}

