#ifndef LKV373VIDEOAPPSRC_H
#define LKV373VIDEOAPPSRC_H

#include <QObject>
#include <QByteArray>
#include <QTime>
#include <QQueue>

#include <QGst/Memory>
#include <QGst/Buffer>
#include <QGst/Utils/ApplicationSource>

class LKV373VideoAppSrc : public QObject, public QGst::Utils::ApplicationSource
{
    Q_OBJECT

public:
    explicit LKV373VideoAppSrc(QObject *parent = 0);
    ~LKV373VideoAppSrc();

private:
    QTime timer;
    QQueue<qreal> frameDiffs;

    qreal calculateFps();

public slots:
    void newFrame(QByteArray frameData);
};

#endif // LKV373VIDEOAPPSRC_H
