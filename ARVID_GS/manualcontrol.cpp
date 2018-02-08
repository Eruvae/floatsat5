#include "manualcontrol.h"
#include "ui_manualcontrol.h"
#include <cmath>

ManualControl::ManualControl(QWidget *parent, SatelliteLink *link) :
    QDialog(parent),
    ui(new Ui::ManualControl),
    link(link),
    gamepad(0),
    wheelSpeed(0)
{
    ui->setupUi(this);

    gamepad = new QGamepad(0, this);

    connect(gamepad, SIGNAL(buttonYChanged(bool)), this, SLOT(sendThrusters()));
    connect(gamepad, SIGNAL(buttonBChanged(bool)), this, SLOT(sendThrusters()));
    connect(gamepad, SIGNAL(buttonXChanged(bool)), this, SLOT(sendThrusters()));
    connect(gamepad, SIGNAL(buttonL2Changed(double)), this, SLOT(sendReactionWheel()));
    connect(gamepad, SIGNAL(buttonR2Changed(double)), this, SLOT(sendReactionWheel()));
    connect(gamepad, SIGNAL(axisRightXChanged(double)), this, SLOT(sendThrustersAdvanced()));
    connect(gamepad, SIGNAL(axisRightYChanged(double)), this, SLOT(sendThrustersAdvanced()));
    connect(gamepad, SIGNAL(buttonL3Changed(bool)), this, SLOT(stopReactionWheel(bool)));
}

ManualControl::~ManualControl()
{
    delete ui;
}

void ManualControl::sendThrusters(/*short uID, QList<XboxOneButtons> pressedButtons*/)
{
    qDebug() << "Gamepad slot executed!" << endl;

    Telecommand send;
    send.id = 3; //THRUSTER_CONTROL
    uint8_t data = 0;

    if (gamepad->buttonY()) data |= 0b1;
    if (gamepad->buttonB()) data |= 0b10;
    if (gamepad->buttonX()) data |= 0b100;

    send.data.valveControl = data;
    link->write<Telecommand>(TelecommandType, send);
}

void ManualControl::stopReactionWheel(bool stop)
{
    if (stop)
    {
        Telecommand send;
        send.id = 2; // SEND_RW_SPEED
        wheelSpeed = 0;
        send.data.wheel_target_speed = wheelSpeed;
        link->write<Telecommand>(TelecommandType, send);
    }
}

void ManualControl::sendReactionWheel()
{
    Telecommand send;
    send.id = 2; // SEND_RW_SPEED
    double lspeed = gamepad->buttonL2();
    double rspeed = gamepad->buttonR2();
    qDebug() << lspeed << ", " << rspeed << endl;
    wheelSpeed += (lspeed - rspeed) * 300;
    wheelSpeed = wheelSpeed > 8000 ? 8000 : wheelSpeed < -8000 ? -8000 : wheelSpeed;
    send.data.wheel_target_speed = wheelSpeed;//(lspeed - rspeed) * 8000;
    link->write<Telecommand>(TelecommandType, send);
}

void ManualControl::sendThrustersAdvanced()
{
    double xaxis = gamepad->axisRightX();
    double yaxis = gamepad->axisRightY();

    double strength =  fabs(xaxis*xaxis + yaxis*yaxis);

    Telecommand send;
    send.id = 3; //THRUSTER_CONTROL
    uint8_t data = 0;

    if (strength > 0.5)
    {
        double angle = atan2(-yaxis, xaxis) * 180 / M_PI;
        //qDebug() << angle << endl;
        if (angle < 120 && angle >= 60)
            data = 0b110;
        else if (angle < 60 && angle >= 0)
            data = 0b010;
        else if (angle < 0 && angle >= -60)
            data = 0b011;
        else if (angle < -60 && angle >= -120)
            data = 0b001;
        else if (angle < -120)
            data = 0b101;
        else // angle > 120
            data = 0b100;
    }

    send.data.valveControl = data;
    link->write<Telecommand>(TelecommandType, send);
}
