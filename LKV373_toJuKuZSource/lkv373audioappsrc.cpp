#include "lkv373audioappsrc.h"

LKV373AudioAppSrc::LKV373AudioAppSrc(QObject *parent) : QObject(parent)
{
}

LKV373AudioAppSrc::~LKV373AudioAppSrc()
{
}

void LKV373AudioAppSrc::newChunk(QByteArray chunkData)
{
    QGst::BufferPtr buffer = QGst::Buffer::create(chunkData.size());
    QGst::MapInfo mapInfo;
    buffer->map(mapInfo, QGst::MapWrite);
    memcpy(mapInfo.data(), chunkData.data(), mapInfo.size());
    buffer->unmap(mapInfo);
    pushBuffer(buffer);
}
