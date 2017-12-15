#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::SetupRWSpeedMeter()
{

ui->RWSpeed->setMaxValue(350);
ui->RWSpeed->setMinValue(-350);
ui->RWSpeed->setSteps(10);
ui->RWSpeed->setThresholdEnabled(0);
ui->RWSpeed->setForeground(QColor(255, 255, 255));
ui->RWSpeed->setBackground(QColor(70, 70, 70));
ui->RWSpeed->setLabel("Angular Velocity");
ui->RWSpeed->setUnits("RPM");

}
