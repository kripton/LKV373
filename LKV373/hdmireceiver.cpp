#include "hdmireceiver.h"

HdmiReceiver::HdmiReceiver(QObject *parent, QHostAddress address) :
    QObject(parent)
{
    parseFrames = false;
    emitInvalidFrames = false;

    this->address = address;

    // The control data is well-formed, so we can use a normal QUdpSocket for that
    udpControlDataSocket = new QUdpSocket(this);
    qDebug() << "bind controlsocket:" << udpControlDataSocket->bind(48689, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(udpControlDataSocket, SIGNAL(readyRead()), this, SLOT(readControlDatagram()));
    foreach (QNetworkInterface itf, QNetworkInterface::allInterfaces())
    {
        qDebug() << "JOIN" << udpControlDataSocket->joinMulticastGroup(QHostAddress("226.2.2.2"), itf);
    }

    // The actual data receiver will be started as soon as the first control packet arrives
    receiver = 0;
}

HdmiReceiver::~HdmiReceiver()
{
    // It's actually a bad idea to leave the multicast-group here since other instances might still run
    emit stopDataReceiver();
}

void HdmiReceiver::readControlDatagram()
{
    while (udpControlDataSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpControlDataSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpControlDataSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        if ((address != QHostAddress::Any) && (address != sender))
        {
            return;
        }

        if (senderPort != 48689)
        {
            return;
        }

        quint16 packetNumber;
        packetNumber = (quint8)datagram.data()[8] + ((quint16)datagram.data()[9]) * 256;

        bool link = false;
        if (datagram.data()[27] == 0x03)
        {
            link = true;
        }
        quint16 frameWidth = ((quint16)datagram.data()[28]) * 256 + (quint8)datagram.data()[29];
        quint16 frameHeight = ((quint16)datagram.data()[30]) * 256 + (quint8)datagram.data()[31];
        qreal   frameFPS = (((quint16)datagram.data()[32]) * 256 + (quint8)datagram.data()[33]) / 10.0;

        qDebug() << "CONTROL DATAGRAM from" << sender.toString() << "controlFrameCount" << packetNumber << "LINK:" << link << QString("%1x%2@%3").arg(frameWidth).arg(frameHeight).arg(frameFPS);
        emit controlPacketReceived(sender, link, frameWidth, frameHeight, frameFPS);

        if (receiver == 0)
        {
            receiver = new dataReceiver();
            if (!receiver->init(sender))
            {
                qCritical() << "DataReceiver failed to init. Are you running as root?";
            }

            receiver->moveToThread(&receiverThread);
            connect(this, SIGNAL(startDataReceiver()), receiver, SLOT(startReceive()));
            connect(this, SIGNAL(stopDataReceiver()), receiver, SLOT(stopReceive()));
            connect(receiver, SIGNAL(newVideoFrame(QByteArray)), this, SLOT(recvNewVideoFrame(QByteArray)));
            connect(receiver, SIGNAL(newAudioChunk(QByteArray)), this, SLOT(recvAudioChunk(QByteArray)));
            receiverThread.start();
            emit startDataReceiver();
        }

        QByteArray toSend = QByteArray::fromHex("5446367a600200000000000303010026000000000d2fd8");
        packetNumber += 3;
        toSend.data()[8] = packetNumber % 256;
        toSend.data()[9] = packetNumber / 256;
        udpControlDataSocket->writeDatagram(toSend, sender, senderPort);
    }
}

void HdmiReceiver::recvNewVideoFrame(QByteArray frameData, bool frameValid)
{
    if (!emitInvalidFrames && !frameValid)
    {
        // InvalidFrames are unwanted and frame was considered invalid (chunks missing)
        return;
    }

    if (parseFrames)
    {
        QPixmap pixmap;
        if (!pixmap.loadFromData(frameData))
        {
            // Frame data invalid (according to the parser), do NOT emit it
            return;
        }
    }

    emit newVideoFrame(frameData);
}

void HdmiReceiver::recvAudioChunk(QByteArray audioData)
{
    emit newAudioChunk(audioData);
}
