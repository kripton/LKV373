#include "lkv373appsrc.h"

LKV373AppSrc::LKV373AppSrc(QObject *parent) : QObject(parent)
{
    for (int i = 0; i < 10; i++)
    {
        frameDiffs.enqueue(0.0);
    }
    timer.start();
}

LKV373AppSrc::~LKV373AppSrc()
{
}

qreal LKV373AppSrc::calculateFps()
{
    qreal fps;
    for (int i = 0; i < 10; i++)
    {
        fps += frameDiffs.at(i);
    }
    return 1.0/fps*10000.0;
}

void LKV373AppSrc::newFrame(QByteArray frameData)
{
    frameDiffs.enqueue(timer.elapsed());
    frameDiffs.dequeue();
    timer.restart();

    QGst::BufferPtr buffer = QGst::Buffer::create(frameData.size());
    QGst::MapInfo mapInfo;
    buffer->map(mapInfo, QGst::MapWrite);
    memcpy(mapInfo.data(), frameData.data(), mapInfo.size());
    buffer->unmap(mapInfo);
    pushBuffer(buffer);
}
