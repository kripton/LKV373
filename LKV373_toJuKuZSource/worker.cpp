#include "worker.h"

Worker::Worker(QString sender_address, QObject *parent) :
    QObject(parent)
{
    recv = new HdmiReceiver(0, QHostAddress(sender_address));

    recv->parseFrames = true;
    forcedFramerate = 0;
    outgoingIP = "127.0.0.1";

    connect(recv, SIGNAL(controlPacketReceived(QHostAddress,bool,quint16,quint16,qreal)), this, SLOT(onControlPacket(QHostAddress,bool,quint16,quint16,qreal)));
}

void Worker::onControlPacket(QHostAddress sender, bool link, quint16 width, quint16 height, qreal fps)
{
    if (!link)
    {
        return;
    }

    if (pipeline.isNull())
    {
        if (forcedFramerate != 0)
        {
            fps = forcedFramerate - 0.5;
        }

        quint16 port = 6000 + (sender.toIPv4Address() & 0xFF);

        QString pipeDesc = QString(" appsrc name=videosrc caps=\"%1\" is-live=true do-timestamp=true format=time ! "
                                   " jpegparse ! videorate ! matroskamux streamable=true name=mux ! tcpserversink host=%2 port=%3"
                                   " audiotestsrc ! audio/x-raw,channels=2,rate=48000 ! vorbisenc ! mux.")
                .arg(QString("image/jpeg,width=%1,height=%2,framerate=%3/1").arg(width).arg(height).arg((int)(fps+0.5)))
                .arg(outgoingIP)
                .arg(port);

        qDebug() << "PIPE:" << pipeDesc;

        pipeline = QGst::Parse::launch(pipeDesc).dynamicCast<QGst::Pipeline>();

        videoappsrc.setElement(pipeline->getElementByName("videosrc"));
        connect(recv, SIGNAL(newVideoFrame(QByteArray)), &videoappsrc, SLOT(newFrame(QByteArray)));

        QGlib::connect(pipeline->bus(), "message", this, &Worker::onBusMessage);
        pipeline->bus()->addSignalWatch();
        pipeline->setState(QGst::StatePlaying);
    }
}

void Worker::onBusMessage(const QGst::MessagePtr &message)
{
    qDebug() << "MESSAGE" << message->type() << message->typeName();
    switch (message->type()) {
    case QGst::MessageError: //Some error occurred.
        qCritical() << message.staticCast<QGst::ErrorMessage>()->error() << message.staticCast<QGst::ErrorMessage>()->debugMessage();
        break;
    case QGst::MessageWarning:
        qWarning() << message.staticCast<QGst::WarningMessage>()->error() << message.staticCast<QGst::WarningMessage>()->debugMessage();
        break;
    case QGst::MessageStateChanged: //The element in message->source() has changed state
        qDebug() << "Pipeline NewState" << message.staticCast<QGst::StateChangedMessage>()->newState();
        break;
    default:
        break;
    }
}
