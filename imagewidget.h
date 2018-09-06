// imagewidget.h
#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QtWidgets/QWidget>
#include <QLabel>
#include <QPushButton>
#include <mainwindow.h>
#include <QPoint>
#include <QListWidget>
class QStringList;
class QListWidgetItem;
class QImage;
class QGestureEvent;
class picListShow;
class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    ImageWidget(QWidget *parent = 0);
    ~ImageWidget();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void gestureEvent(QGestureEvent *event);
    //bool event(QEvent *event);
    void touchEvent(QEvent *event);
private:
    QPoint m_mouseSrcPos;   //滑动的初始点
    QPoint m_mouseDstPos;   //滑动的终点
    QLabel *total_label;    //图片滑动的动态页面/放大后的移动页面（大小变化）
    bool mousePress;
    bool mouseMove;
    bool label_move;
    bool firstPress;
    int curIndex;
    int curPosX;
    bool isSingleItemUI;    //是否图片查看界面 default:false
    bool isFirstDouble;     //是否第一次双击   default:true
    bool isZoomMode;        //是否缩放模式    default:false
    int cenPixW;
    int cenPixH;
    double zoomScale;
    int xPosLast;
    int yPosLast;

    void setLabelMove(bool enable);
    void zoomOut(void);     //缩小图片
    void zoomIn(void);      //放大图片
    void updateLoadImg(int index);
    double getScaleValue(QSize img, QSize view);

private slots:
    void slot_itemClicked(QListWidgetItem*);
    void menuView(void);
    void back2Album(void);

private:
    QStringList m_imgList;                  // 文件目录下所有的图像文件名
    QString m_strPath;                      // 文件夹路径
    QLabel *mShowWidget;                    // 图像显示窗口
    QPushButton *menuButton;
    QPushButton *backButton;
    QPixmap cenPixmap;
    QPixmap showPixmap;
    QImage  cenImg;
    picListShow *pListShow;
};

class picListShow : public QListWidget{
    Q_OBJECT
public:
    explicit picListShow(QWidget *parent);
    ~picListShow();
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    int yPos0;
    int yPos1;
    int xPos0;
    int xPos1;
    QPoint slidePoint;
    ImageWidget *imageP;
};

#endif // IMAGEWIDGET_H
