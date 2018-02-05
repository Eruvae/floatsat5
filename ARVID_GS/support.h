#ifndef SUPPORT_H
#define SUPPORT_H
#include "satellitelink.h"

#include <QDialog>

namespace Ui {
class Support;
}

class Support : public QDialog
{
    Q_OBJECT

public:
    /*explicit*/ Support(QWidget *parent = 0,  SatelliteLink *link = 0);
    ~Support();
    SatelliteLink *TCLink;

private:
    Ui::Support *ui;

public slots:
    void telecommand();



private slots:
    void on_pushButton_clicked();
};

#endif // SUPPORT_H
