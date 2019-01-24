#include "mainwidget.h"
#include <QApplication>
#include <albumwidget.h>
#include <QCommandLineParser>
#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("album");
    a.setApplicationVersion(APP_VERSION);
    QCommandLineParser parser;
    parser.addVersionOption();

    if(argc > 1){
        if((strcmp(argv[1],"--version") == 0) || (strcmp(argv[1],"-v") == 0)){
            parser.process(a);
            return a.exec();
        }
    }

    QList<Qt::GestureType> gestures;
    gestures << Qt::PanGesture;
    gestures << Qt::PinchGesture;
    gestures << Qt::SwipeGesture;

    MainWidget w;
//    w.grabGestures(gestures);
    w.show();

    return a.exec();
}
