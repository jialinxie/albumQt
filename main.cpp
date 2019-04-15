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

    MainWidget w;
    w.show();

    return a.exec();
}
