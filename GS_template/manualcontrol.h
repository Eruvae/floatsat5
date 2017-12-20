#ifndef MANUALCONTROL_H
#define MANUALCONTROL_H

#include <QDialog>
#include "satellitelink.h"
#include <QGamepad>

namespace Ui {
class ManualControl;
}

class ManualControl : public QDialog
{
    Q_OBJECT

public:
    explicit ManualControl(QWidget *parent = 0, SatelliteLink *link = 0);
    ~ManualControl();

private:
    Ui::ManualControl *ui;
    SatelliteLink *link;
    QGamepad *gamepad;

private slots:
    void sendThrusters();
    void sendReactionWheel();
    void sendThrustersAdvanced();
};

#endif // MANUALCONTROL_H
