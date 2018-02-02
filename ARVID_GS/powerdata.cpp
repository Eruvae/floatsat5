#include "powerdata.h"
#include "ui_powerdata.h"

extern float voltagema, currentma,voltagemb, currentmb,voltagemc, currentmc,voltagemd, currentmd;
extern bool valve1, valve2, valve3;

PowerData::PowerData(QMainWindow *parent, SatelliteLink *link) :
    QDialog(parent),
    ui(new Ui::PowerData),
    PDlink(link)
{
    ui->setupUi(this);
    QPixmap pix(":/Img/thrust.png");
    ui->label_9->setPixmap(pix);

    connect(parent, SIGNAL(powerDataUpdate()), this, SLOT(updatePowerData()));

}

PowerData::~PowerData()
{
    delete ui;
}

void PowerData::updatePowerData()
{
    ui->CurrentRW->display(currentma);
    ui->PowerRW->display((currentma*voltagema)/1000);
    ui->CurrentT1->display(currentmc);
    ui->PowerT1->display((currentmc*voltagemc)/1000);
    if((valve2&&valve3) || (!valve2 && !valve3))
    {
     ui->CurrentT2->display(currentmb/2);
     ui->CurrentT3->display(currentmb/2);
     ui->PowerT2->display(((currentmb*voltagemb)/2)/1000);
     ui->PowerT3->display(((currentmb*voltagemb)/2)/1000);
    }
    else if (valve2)
    {
        ui->CurrentT2->display(currentmb);
        ui->CurrentT3->display(0);
        ui->PowerT2->display((currentmb*voltagemb)/1000);
        ui->PowerT3->display(0);
    }

    else if (valve3)
    {
        ui->CurrentT3->display(currentmb);
        ui->CurrentT2->display(0);
        ui->PowerT3->display((currentmb*voltagemb)/1000);
        ui->PowerT2->display(0);
    }

}
