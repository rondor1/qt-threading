#include <QApplication>
#include "mandlebrotwidget.h"

int main (int argc, char** argv)
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);
    MandlebrotWidget widget;
    widget.show();
    return app.exec();
}
