#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

class QLabel;
class QPushButton;
class Block;
class RenderThread;

class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr);
    void loadImage(const QImage &image);

public slots:
    void addBlock(const Block& block);

private slots:
    void loadImage();
    void resetUi();

private:
    RenderThread* thread;
    QLabel* label;
    QPixmap pixmap;
    QPushButton* loadButton;
    QPushButton* resetButton;
    QString path;

};

#endif // WINDOW_H
