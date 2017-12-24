#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->speedmeter->setMaxValue(350);
    ui->speedmeter->setMinValue(-350);
    ui->speedmeter->setThresholdEnabled(0);
    ui->speedmeter->setForeground(QColor(255, 255, 255));
    ui->speedmeter->setBackground(QColor(0, 0, 0));
    //Set up LED
    ui->led->setState(1);
    ui->led->setLedSize(30);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->label->setText("Clicked!");
}
