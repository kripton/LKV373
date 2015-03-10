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

#include "lkv373audioappsrc.h"
#include "lkv373videoappsrc.h"

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
    LKV373VideoAppSrc videoappsrc;
    LKV373AudioAppSrc audioappsrc;
    QGst::PipelinePtr pipeline;

    void onBusMessage(const QGst::MessagePtr & message);
};

#endif // WORKER_H
