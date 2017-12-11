#ifndef GSTATIONLINK_H
#define GSTATIONLINK_H

#include <QUdpSocket>
#include <QTcpSocket>
#include <QQueue>
#include <QSet>
#include <QTimer>
#include "payload.h"

class GStationlink : public QObject
{
    Q_OBJECT

    QHostAddress localAddress;
    QHostAddress remoteAddress;
    quint16 port;
    //QUdpSocket socket;
    QTcpSocket socket;
    bool bound;
    //bool checkChecksum;
    QSet<quint32> topics;
    //qint64 receivedBytes;
    //qint64 sentBytes;
    //QTimer timer;
signals:
    void readReady();

public:
    explicit GStationlink(QObject *parent = 0);
    void addTopic(PayloadType);
    int write(quint32 topicId, const QByteArray &data);
    int write(quint32 topicId, const Telecommand &telecommand);
    Payload read();
    qint64 readAndResetReceivedBytes();
    qint64 readAndResetSentBytes();
    bool isBound();
};



#endif // GSTATIONLINK_H
