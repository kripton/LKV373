#ifndef LKV373AUDIOAPPSRC_H
#define LKV373AUDIOAPPSRC_H

#include <QObject>
#include <QByteArray>
#include <QTime>

#include <QGst/Memory>
#include <QGst/Buffer>
#include <QGst/Utils/ApplicationSource>

class LKV373AudioAppSrc : public QObject, public QGst::Utils::ApplicationSource
{
    Q_OBJECT

public:
    explicit LKV373AudioAppSrc(QObject *parent = 0);
    ~LKV373AudioAppSrc();

private:
    QTime timer;

public slots:
    void newChunk(QByteArray chunkData);
};
#endif // LKV373AUDIOAPPSRC_H
