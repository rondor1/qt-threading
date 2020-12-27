#include "mandlebrotwidget.h"

#include <QPainter>
#include <QKeyEvent>
#include <cmath>

const double DefaultCenterX = -0.637011;
const double DefaultCenterY = -0.0395159;
const double DefaultScale = 0.00403897;

const double ZoomInFactor = 0.8;
const double ZoomOutFactor = 1 / ZoomInFactor;
const int ScrollStep = 20;

MandlebrotWidget::MandlebrotWidget(QWidget *parent) : QWidget(parent),
    centerX(DefaultCenterX),
    centerY(DefaultCenterY),
    pixmapScale(DefaultScale),
    curScale(DefaultScale)
{

    connect(&thread, &RenderThread::renderedImage, this, &MandlebrotWidget::updatePixmap);
    setWindowTitle("Mandelbrot");
#if QT_CONFIG(cursor)
    setCursor(Qt::CrossCursor);
#endif
    resize(550, 400);
}

void MandlebrotWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    if(pixmap.isNull())
    {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, tr("Rendering initial image, please wait..."));
        return;
    }

    if(qFuzzyCompare(curScale, pixmapScale))
    {
        painter.drawPixmap(rect(), pixmap);
    }
    else
    {
        //Draw preview pixmap, if it's not ready
        auto previewPixmap = qFuzzyCompare(pixmap.devicePixelRatio(), qreal(1))?
                    pixmap :
                    pixmap.scaled(pixmap.size() / pixmap.devicePixelRatioF(), Qt::KeepAspectRatio,
                                  Qt::SmoothTransformation);

        double scaleFactor = pixmapScale / curScale;
        int newWidth = int(previewPixmap.width() * scaleFactor);
        int newHeight = int(previewPixmap.height() * scaleFactor);
        int newX = pixmapOffset.x() + (previewPixmap.width() - newWidth) / 2;
        int newY = pixmapOffset.y() + (previewPixmap.height() - newHeight) / 2;

        painter.save();
        painter.translate(newX, newY);
        painter.scale(scaleFactor, scaleFactor);

        QRectF exposed = painter.transform().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
        painter.drawPixmap(exposed, previewPixmap, exposed);
        painter.restore();

        QString text = tr("Use mouse wheel or the '+' and '-' keys to zoom. "
                          "Press and hold left mouse button to scroll.");
        QFontMetrics metrics = painter.fontMetrics();
        int textWidth = metrics.horizontalAdvance(text);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 127));
        painter.drawRect((width() - textWidth) / 2 - 5, 0, textWidth + 10, metrics.lineSpacing() + 5);
        painter.setPen(Qt::white);
        painter.drawText((width() - textWidth) / 2, metrics.leading() + metrics.ascent(), text);
    }
}

#if QT_CONFIG(wheelevent)
void MandlebrotWidget::wheelEvent(QWheelEvent *event)
{
    //Get the number of rotations of the wheel, on y axis
    // Angle is returned in eights of a degree.
    const int numOfDegrees = event->angleDelta().y()/8;
    //Get the numbers of steps made, as one moving of mouse is 15 degrees
    const double numSteps = double(numOfDegrees) / 15.0;
    //Zoom in based on a steps of 8.
    zoom(pow(ZoomInFactor, numSteps));
}
#endif

void MandlebrotWidget::resizeEvent(QResizeEvent *event)
{
    thread.render(centerX, centerY, curScale, size(), devicePixelRatioF());
}

void MandlebrotWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Plus:
        zoom(ZoomInFactor);
        break;
    case Qt::Key_Minus:
        zoom(ZoomOutFactor);
        break;
    case Qt::Key_Left:
        scroll(-ScrollStep, 0);
        break;
    case Qt::Key_Right:
        scroll(+ScrollStep, 0);
        break;
    case Qt::Key_Up:
        scroll(0, -ScrollStep);
        break;
    case Qt::Key_Down:
        scroll(0, +ScrollStep);
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

void MandlebrotWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        //Get the last click position
        lastDragPos = event->pos();
    }
}

void MandlebrotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        pixmapOffset += event->pos() - lastDragPos;
        lastDragPos = event->pos();
        update();
    }
}

void MandlebrotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        pixmapOffset += event->pos() - lastDragPos;
        lastDragPos = QPoint();

        const auto pixmapSize = pixmap.size() / pixmap.devicePixelRatioF();
        int deltaX = (width() - pixmapSize.width()) / 2 - pixmapOffset.x();
        int deltaY = (height() - pixmapSize.height()) / 2 - pixmapOffset.y();
        scroll(deltaX, deltaY);
    }
}

void MandlebrotWidget::updatePixmap(const QImage &image, double scaleFactor)
{
    if (!lastDragPos.isNull())
            return;

        pixmap = QPixmap::fromImage(image);
        pixmapOffset = QPoint();
        lastDragPos = QPoint();
        pixmapScale = scaleFactor;
        update();
}

void MandlebrotWidget::zoom(double zoomFactor)
{
    curScale *= zoomFactor;
    update();
    thread.render(centerX, centerY, curScale, size(), devicePixelRatioF());
}

void MandlebrotWidget::scroll(int deltaX, int deltaY)
{
    centerX += deltaX * curScale;
    centerY += deltaY * curScale;
    update();
    thread.render(centerX, centerY, curScale, size(), devicePixelRatioF());
}
