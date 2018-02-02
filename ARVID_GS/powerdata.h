#ifndef POWERDATA_H
#define POWERDATA_H

#include <QDialog>
#include "satellitelink.h"
#include "mainwindow.h"
namespace Ui {
class PowerData;
}

class PowerData : public QDialog
{
    Q_OBJECT

public:
    /*explicit*/ PowerData(QMainWindow *parent = 0, SatelliteLink *link=0);
    ~PowerData();

private:
    Ui::PowerData *ui;
    SatelliteLink *PDlink;

public slots:
    void updatePowerData();
};

#endif // POWERDATA_H
