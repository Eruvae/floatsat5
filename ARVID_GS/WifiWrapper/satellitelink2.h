#ifndef Satellitelink2_H
#define Satellitelink2_H

#endif // Satellitelink2_H

#include <QUdpSocket>
#include <QTcpSocket>
#include <QQueue>
#include <QSet>
#include <QTimer>
#include "payload.h"

class Satellitelink2 : public QObject
{
    Q_OBJECT

    QHostAddress localAddress;
    QHostAddress remoteAddress;
    quint16 port;
    QUdpSocket socket;
   // QTcpSocket socket;
    bool bound;
    bool checkChecksum;
    QSet<quint32> topics;
    qint64 receivedBytes;
    qint64 sentBytes;
    QTimer timer;
signals:
    void readReady();

public:
    explicit Satellitelink2(QObject *parent = 0, bool checkChecksum = true);
    void addTopic(PayloadType);
    int write(quint32 topicId, const QByteArray &data);
    int write(quint32 topicId, const Telecommand &telecommand);
    Payload read();
    qint64 readAndResetReceivedBytes();
    qint64 readAndResetSentBytes();
    bool isBound();
};

#endif // Satellitelink2_H
