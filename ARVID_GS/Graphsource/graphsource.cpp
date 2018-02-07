

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>



void MainWindow::SetupGraphCurrent()

{

    ui->graph_temp->addGraph();
    ui->graph_temp->graph(0)->setPen(QPen(QColor(255, 0, 0)));
    ui->graph_temp->setBackground(Qt::black);
    ui->graph_temp->axisRect()->setBackground(Qt::black);

    ui->graph_temp->xAxis->setLabel("Time Elapsed");
    ui->graph_temp->xAxis->setLabelColor(QColor(255, 255, 255));
    ui->graph_temp->yAxis->setLabel("Current (A)");
    ui->graph_temp->yAxis->setLabelColor(QColor(255, 255, 255));
    ui->graph_temp->yAxis->setTickLabelColor(Qt::white);
    ui->graph_temp->xAxis->setTickLabelColor(Qt::white);

    ui->graph_temp->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->graph_temp->xAxis->setDateTimeFormat("mm:ss");
    ui->graph_temp->xAxis->setAutoTickStep(false);
    ui->graph_temp->xAxis->setTickStep(2);
    ui->graph_temp->axisRect()->setupFullAxesBox();
    ui->graph_temp->yAxis->setRange(0, 1.5);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->graph_temp->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->graph_temp->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->graph_temp->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->graph_temp->yAxis2, SLOT(setRange(QCPRange)));
    QTimer dataTimer;



    dataTimer.start(0); // Interval 0 means to refresh as fast as possible

}


void MainWindow::SetupRealtimeDataSlotCurrent(double newValue)
{

double key = QTime::currentTime().msecsSinceStartOfDay()/1000.0; // time elapsed since start of demo, in seconds
static double lastPointKey = 0;
if (key-lastPointKey > 0.01) //
{

  ui->graph_temp->graph(0)->addData(key, newValue);
  ui->graph_temp->graph(0)->rescaleAxes(true);
  lastPointKey = key;
}

ui->graph_temp->xAxis->setRange(key, 8, Qt::AlignRight);
ui->graph_temp->replot();


static double lastFpsKey;
static int frameCount;
++frameCount;
if (key-lastFpsKey > 2)
{
  ui->statusBar->showMessage(
        QString("%1 FPS, Total Data points: %2")
        .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
        .arg(ui->graph_temp->graph(0)->data()->size()) , 0);
  lastFpsKey = key;
  frameCount = 0;
}

}



