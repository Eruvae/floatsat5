#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

void MainWindow::SetupSpeedMeter()
{
    ui->speedmeter->setMaxValue(8000);
    ui->speedmeter->setMinValue(-8000);
    ui->speedmeter->setThresholdEnabled(0);
    ui->speedmeter->setForeground(QColor(255, 255, 255));
    ui->speedmeter->setBackground(QColor(0, 0, 0));
}
