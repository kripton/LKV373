#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&scene);
    pixmapItem = scene.addPixmap(QPixmap());

    connect(ui->listWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(selectedItemChanged(QString)));

    for (int i = 0; i < 10; i++)
    {
        frameDiffs.enqueue(0.0);
    }
    timer.start();

    worker = new HdmiReceiver(0);

    worker->parseFrames = false;

    connect(worker, SIGNAL(newVideoFrame(QByteArray)), this, SLOT(processVideoFrame(QByteArray)));
    connect(worker, SIGNAL(newAudioChunk(QByteArray)), this, SLOT(processAudioChunk(QByteArray)));
    connect(worker, SIGNAL(controlPacketReceived(QHostAddress,bool,quint16,quint16,qreal)), this, SLOT(newControlPacket(QHostAddress,bool,quint16,quint16,qreal)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F)
    {
        if (this->isFullScreen())
        {
            this->showNormal();
            ui->graphicsView->setGeometry(oldSize);
            ui->label->show();
            ui->listWidget->show();
        }
        else
        {
            this->showFullScreen();
            oldSize = ui->graphicsView->geometry();
            ui->graphicsView->setGeometry(QApplication::desktop()->screenGeometry(this));
            ui->label->hide();
            ui->listWidget->hide();
        }
        return;
    }
    QWidget::keyPressEvent(event);
}

qreal MainWindow::calculateFps()
{
    qreal fps;
    for (int i = 0; i < 10; i++)
    {
        fps += frameDiffs.at(i);
    }
    return 1.0/fps*10000.0;
}

void MainWindow::processVideoFrame(QByteArray frameData)
{
    frameDiffs.enqueue(timer.elapsed());
    frameDiffs.dequeue();
    timer.restart();

    ui->label->setText(QString("%1 FPS").arg(calculateFps(), 3, 'f', 4));

    QImage image;
    if (!image.loadFromData(frameData))
    {
        return;
    }
    image = image.scaled(ui->graphicsView->width(), ui->graphicsView->height(), Qt::KeepAspectRatio, Qt::FastTransformation);

    pixmapItem->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::processAudioChunk(QByteArray audioData)
{
    // Audio data is S32BE, 2ch interleaved, 48kHz
    qint32 leftMax = 0;
    qint32 rightMax = 0;
    for (int i = 0; i < audioData.size(); i = i + 8)
    {
        qint32 leftValue = qFromBigEndian<qint32>((const uchar*)(audioData.data() + i));
        qint32 rightValue = qFromBigEndian<qint32>((const uchar*)(audioData.data() + i + 4));
        leftMax = qMax(leftMax, leftValue);
        rightMax = qMax(rightMax, rightValue);
    }

    // The values divided by don't really make sense. It's 2^24/10 and okay since
    // it's not a real Peak- or VU-meter but only to display something at least
    ui->leftAudioSlider->setValue(leftMax/1677721);
    ui->rightAudioSlider->setValue(rightMax/1677721);
}

void MainWindow::newControlPacket(QHostAddress sender, bool link, quint16 width, quint16 height, qreal fps)
{
    Q_UNUSED(link)

    QString newText = QString("%1 (%2x%3@%4)").arg(sender.toString()).arg(width).arg(height).arg(fps);

    foreach (QListWidgetItem* item, ui->listWidget->findItems("*", Qt::MatchWildcard))
    {
        if (item->text().startsWith(sender.toString()))
        {
            item->setText(newText);
            return;
        }
    }

    ui->listWidget->addItem(newText);
}

void MainWindow::selectedItemChanged(QString newText)
{
    QHostAddress newSender = QHostAddress(newText.split(" ")[0]);
    worker->setSender(newSender);
}
