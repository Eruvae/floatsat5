

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>


double Square_Wave() {
   const int half_period=5;
   double amplitude= -5;
   int i=0;
   int k;
   double x[100];

   char command;
   float data;
   char buffer[50];
   sprintf(buffer,"Hello\r\n");
   //for(k=0;k<100;k++)

    if (!(i++ % half_period))
    {
    amplitude =-amplitude;
  }

    x[k]=amplitude;
      printf("%d\t %12.7f\n",k,x[k]);
    return amplitude;
}



void MainWindow::setupGraph()

{

    ui->graph_temp->setBackground(Qt::lightGray);
     ui->graph_temp->addGraph(); // blue line
     ui->graph_temp->graph(0)->setPen(QPen(Qt::darkGreen));
    // ui->graph_temp->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
     ui->graph_temp->graph(0)->setAntialiasedFill(false);



     ui->graph_temp->addGraph(); // blue dot
     ui->graph_temp->graph(1)->setPen(QPen(Qt::blue));
     ui->graph_temp->graph(1)->setLineStyle(QCPGraph::lsNone);
     ui->graph_temp->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);


       ui->graph_temp->setBackground(QColor(224,224,224));
     ui->graph_temp->xAxis->setTickLabelType(QCPAxis::ltDateTime);
     ui->graph_temp->xAxis->setDateTimeFormat("hh:mm:ss");
     ui->graph_temp->xAxis->setAutoTickStep(false);
     ui->graph_temp->xAxis->setTickStep(2);
     ui->graph_temp->axisRect()->setupFullAxesBox();

     ui->graph_temp->xAxis->setLabel("Time");
     ui->graph_temp->yAxis->setLabel("Packets Status");
     //ui->graph_temp->setWindowTitle("A");

     ui->graph_temp->yAxis->setBasePen(QPen(Qt::black));
     ui->graph_temp->yAxis->setRange(0,0);

    ui->graph_temp->yAxis->setTicks(false);

    ui->graph_temp->yAxis->setTickLabels(false);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect( ui->graph_temp->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->graph_temp->xAxis2, SLOT(setRange(QCPRange)));
  //  connect( ui->graph_temp->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->graph_temp->yAxis2, SLOT(setRange(QCPRange)));

    QTimer dataTimer;
    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  //  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(setupGraphrealtimeDataSlot()));
   dataTimer.start(0); // Interval 0 means to refresh as fast as possible */

}

//value0=qSin(key);

void MainWindow::setupGraphrealtimeDataSlot()

{

// calculate two new data points:
double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
static double lastPointKey = 0;
if (key-lastPointKey > 0.01) // at most add point every 10 ms
{
  // double value0=Square_Wave();
  //double value0=qSin(key*1.6+qCos(key*1.7)*2)*10 + qSin(key*1.2+0.56)*20 + 26;
    double value0=1;

  double value1=2;// = gz; //qSin(key*1.3+qCos(key*1.2)*1.2)*7 + qSin(key*0.9+0.26)*24 + 26;

  //ui->lineEditActualVelocity->setText(QString::number(gz));
  // add data to lines:
  //ui->graph_temp->graph(0)->addData(key, value0);
  //ui->graph_temp->graph(1)->addData(key, value1);
  // set data of dots:
 // ui->graph_temp->graph(2)->clearData();
  ui->graph_temp->graph(1)->addData(key, value0); // done by Faisal
 // ui->graph_temp->graph(3)->clearData();
  //ui->graph_temp->graph(3)->addData(key, value1);
  // remove data of lines that's outside visible range:
 // ui->graph_temp->graph(0)->removeDataBefore(key-8);
 // ui->graph_temp->graph(1)->removeDataBefore(key-8);
  // rescale value (vertical) axis to fit the current data:
 // ui->graph_temp->graph(0)->rescaleValueAxis();
  ui->graph_temp->graph(1)->rescaleValueAxis(true);  // done by Faisal
  lastPointKey = key;
}
// make key axis range scroll with the data (at a constant range size of 8):
ui->graph_temp->xAxis->setRange(key+0.25, 8, Qt::AlignRight);
ui->graph_temp->replot();

//ui->lineEdit_VelocityRPMdisplay->setText(QString::number(motorSpeed));


//ui->lineEditgzDisplay->setText(gz);

// calculate frames per second:
static double lastFpsKey;
static int frameCount;
++frameCount;
if (key-lastFpsKey > 2) // average fps over 2 seconds
{

  ui->statusBar->showMessage(
        QString("%1 FPS, Total Data points: %2")
        .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
        .arg(ui->graph_temp->graph(0)->data()->count()+ui->graph_temp->graph(1)->data()->count())
        , 0);
  lastFpsKey = key;
  frameCount = 0;
}

}


void MainWindow::on_pushButton_clicked()
{
    QString fileName("customPlot.png");
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << file.errorString();
    } else {
        ui->graph_temp->savePng(fileName);

    }
}

