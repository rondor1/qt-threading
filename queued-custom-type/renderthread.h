#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QThread>
#include <QImage>
#include <QMutex>

class Block;

class RenderThread : public QThread
{
    Q_OBJECT
public:
    RenderThread(QObject *parent = nullptr);
    void processImage(const QImage& image);

signals:
    void sendBlock(const Block& block);

public slots:
    void stopProcess();

protected:
    void run();

private:
    bool m_abort;
    QImage m_image;
    QMutex mutex;
};

#endif // RENDERTHREAD_H
