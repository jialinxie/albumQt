#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QMainWindow>
class AlbumWidget;

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    void grabGestures(const QList<Qt::GestureType> &gestures);

private:
    AlbumWidget *imageWidget;
};

#endif // MAINWIDGET_H
