#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QHostAddress>

#include <QGlib/Connect>
#include <QGst/Parse>
#include <QGst/Pipeline>
#include <QGst/Bus>
#include <QGst/Message>

#include "hdmireceiver.h"

#include "lkv373appsrc.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QString sender_address, QObject *parent = 0);
    int forcedFramerate;
    QString outgoingIP;

signals:

private slots:
    void onControlPacket(QHostAddress sender, bool link, quint16 width, quint16 height, qreal fps);

private:
    HdmiReceiver* recv;
    LKV373AppSrc appsrc;
    QGst::PipelinePtr pipeline;

    void onBusMessage(const QGst::MessagePtr & message);
};

#endif // WORKER_H
