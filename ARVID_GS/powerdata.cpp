#include "powerdata.h"
#include "ui_powerdata.h"

extern int16_t voltagema, currentma,voltagemb, currentmb,voltagemc, currentmc,voltagemd, currentmd;

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

}
