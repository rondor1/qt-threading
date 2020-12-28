#include "window.h"

#include "block.h"
#include "renderthread.h"

#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QGuiApplication>
#include <QImageReader>
#include <QFileDialog>
#include <QScreen>

Window::Window(QWidget *parent) : QWidget(parent), thread(new RenderThread(this))
{
    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->setMinimumSize({400, 400});

    loadButton = new QPushButton(tr("&Load image..."), this);

    resetButton = new QPushButton(tr("&Stop"), this);
    resetButton->setEnabled(false);

    connect(loadButton, &QPushButton::clicked, this, QOverload<>::of(&Window::loadImage));
    connect(resetButton, &QPushButton::clicked, thread, &RenderThread::stopProcess);
    connect(thread, &RenderThread::finished, this, &Window::resetUi);
    connect(thread, &RenderThread::sendBlock, this, &Window::addBlock);

    QHBoxLayout* buttonLayout = new QHBoxLayout(this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();


    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addSpacing(buttonLayout->sizeHint().width());
    layout->addLayout(buttonLayout);

    setWindowTitle("Queued custom type");
}

void Window::loadImage(const QImage &image)
{
    QImage useImage;
        QRect space = QGuiApplication::primaryScreen()->availableGeometry();
        if (image.width() > 0.75*space.width() || image.height() > 0.75*space.height())
            useImage = image.scaled(0.75*space.width(), 0.75*space.height(),
                                    Qt::KeepAspectRatio, Qt::SmoothTransformation);
        else
            useImage = image;

        pixmap = QPixmap(useImage.width(), useImage.height());
        pixmap.fill(qRgb(255, 255, 255));
        label->setPixmap(pixmap);
        loadButton->setEnabled(false);
        resetButton->setEnabled(true);
        thread->processImage(useImage);
}

void Window::addBlock(const Block &block)
{
    QColor color{block.color()};
    color.setAlpha(64);

    QPainter painter;
    painter.begin(&pixmap);
    painter.fillRect(block.rect(), color);
    painter.end();
    label->setPixmap(pixmap);
}

void Window::loadImage()
{
    QStringList formats;

    //Acquire a list of all supported image formats
    const QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();

    for (const QByteArray &format : supportedFormats)
    {
        //Add all image formats to the list
        if (format.toLower() == format)
        {
            formats.append(QLatin1String("*.") + QString::fromLatin1(format));
        }
    }

    //Open image dialog based on the provided filters
    QString newPath = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                                   path, tr("Image files (%1)").arg(formats.join(' ')));

    if (newPath.isEmpty())
    {
        return;
    }

    QImage image(newPath);
    if (!image.isNull())
    {
        loadImage(image);
        path = newPath;
    }
}

void Window::resetUi()
{
    loadButton->setEnabled(true);
    resetButton->setEnabled(false);
}

