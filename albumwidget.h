﻿// AlbumWidget.h
#ifndef AlbumWidget_H
#define AlbumWidget_H

#include <QtWidgets/QWidget>
#include <QLabel>
#include <QPushButton>
#include <mainwindow.h>
#include <QPoint>
#include <QListWidget>

#include "QPanGesture"
#include <QDir>

#define ALBUM_PATH      "../Camera/"
//屏幕分辨率，以及标题高度
#define SCREEN_WIDTH              (240)
#define SCREEN_HEIGHT             (320)
#define TOP_TITLE_HEIGHT           (30)
class QStringList;
class QListWidgetItem;
class QImage;
class QGestureEvent;
class picListShow;
class QCheckBox;
class AlbumWidget : public QWidget
{
    Q_OBJECT

public:
    AlbumWidget(QWidget *parent = 0);
    ~AlbumWidget();

protected:
    void focusInEvent(QFocusEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    bool gestureEvent(QGestureEvent *event);
    bool event(QEvent *event);
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

    qreal horizontalOffset;
    qreal verticalOffset;

    qreal scaleFactor;
    qreal currentStepScaleFactor;

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

    int updateUI();
    QDir dir;
    QStringList filters;
    QSize IMAGE_SIZE;
    QSize ITEM_SIZE;

    /**
     * bin 20181107
     * @brief panTriggered
     * @param gesture
     */
    void panTriggered(QPanGesture *gesture);

    void pinchTriggered(QPinchGesture *gesture);
    //缩放因子
    void zoom(float scale);
};

class picListShow : public QListWidget{
    Q_OBJECT
public:
    explicit picListShow(QWidget *parent);
    ~picListShow();
    QList<QCheckBox*> *checkboxList;

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QPoint slidePoint;
};

#endif // AlbumWidget_H