// AlbumWidget.h
#ifndef AlbumWidget_H
#define AlbumWidget_H

#include <QtWidgets/QWidget>
#include <QLabel>
#include <QPushButton>
#include <mainwidget.h>
#include <QPoint>
#include <QListWidget>

#include "QPanGesture"
#include <QDir>
#include <QImage>
#include <QImageReader>
#include <QPainter>
//#include "readimgthread.h"

//获取鼠标坐标函数分类，根据不同情况使用不同函数
#define GET_POS_X                  QCursor::pos().x();//event->globalX()//
#define GET_POS_Y                  QCursor::pos().y();//event->globalX()//

#ifdef QT_PC
    #define ALBUM_PATH      "/Users/jack/Downloads/Camera/"
#else
    #define ALBUM_PATH      "/launcher/BHC_USER/Camera/"
#endif
//屏幕分辨率，以及标题高度
#define SCREEN_WIDTH              (240)
#define SCREEN_HEIGHT             (320)
#define TOP_TITLE_HEIGHT           (30)
#define STATUS_BAR_HEIGHT (35)
//按键设定
#ifdef QT_PC
#define KEY_UP                     Qt::Key_Up
#define KEY_LEFT                   Qt::Key_Left
#define KEY_RIGHT                  Qt::Key_Right
#define KEY_DOWN                   Qt::Key_Down
#define KEY_ENTER                  Qt::Key_Return   //confirm button
#define KEY_RETURN                 Qt::Key_A        //back button
#define KEY_SERACH                 Qt::Key_S
#define KEY_PICKPOINT              Qt::Key_W
#define KEY_SHUTDOWN               Qt::Key_P//Qt::Key_PowerOff
#define KEY_MENU                   Qt::Key_Q
#define KEY_ENLARGE                Qt::Key_Plus
#define KEY_NARROW                 Qt::Key_Minus
#else
#define KEY_UP                     Qt::Key_Up
#define KEY_LEFT                   Qt::Key_Left
#define KEY_RIGHT                  Qt::Key_Right
#define KEY_DOWN                   Qt::Key_Down
#define KEY_ENTER                  Qt::Key_F12
#define KEY_RETURN                 Qt::Key_F6
#define KEY_SERACH                 Qt::Key_F10
#define KEY_PICKPOINT              Qt::Key_F11
#define KEY_SHUTDOWN               Qt::Key_PowerOff//Qt::Key_PowerOff
#define KEY_MENU                   Qt::Key_F5
#define KEY_ENLARGE                Qt::Key_F7
#define KEY_NARROW                 Qt::Key_F8
#endif

class QStringList;
class QListWidgetItem;
class QImage;
class QGestureEvent;
class picListShow;
class QCheckBox;
class singleImgShow;
class AlbumWidget : public QWidget
{
    Q_OBJECT
public:
    static AlbumWidget* getInstance(QWidget *parent){
        static AlbumWidget *a_instance = new AlbumWidget(parent);
        return a_instance;
    }

    AlbumWidget(QWidget *parent = nullptr);
    ~AlbumWidget();
    int updateUI();
    int getCurrentImageCount(void);
    int getOldImageCount(void);
    void setOldImageCount(int count);

    QListWidget *pListShow;

protected:
    void focusInEvent(QFocusEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    bool backEvent();                                   //点击返回按钮式由派生类做判断，要不要返回
    void mouseMoveEvent(QMouseEvent *event);
private:
    QLabel *backgroundLabel;
    singleImgShow *pSingleImgShow;
    QPoint m_mouseSrcPos;   //滑动的初始点
    QPoint m_mouseDstPos;   //滑动的终点
    int curIndex;
    int oldImageCount;

    bool moveStatus(void);
    void setMoveStatus(void);
    double moveLength() const;

private slots:
    void slot_itemClicked(QListWidgetItem*);
    void menuView(void);
    void back2Album(void);
    void albumMenuSlot(int idx);

private:
    QPushButton *menuButton;
    QPushButton *backButton;
    QDir dir;
    QStringList filters;
    QSize IMAGE_SIZE;
    QSize ITEM_SIZE;
    QStringList files;                      // 文件目录下所有的图像文件名
    int position;
    bool isMove;
    QPoint slidePoint;
    double m_moveLength = -1;//移动长度
};

class singleImgShow : public QWidget{
    Q_OBJECT
public:
    explicit singleImgShow(QWidget *parent = nullptr);
    ~singleImgShow();
    void grabGestures(const QList<Qt::GestureType> &gestures);
    QList<QPixmap> pixmapList;
    void goToImage(int index);

protected:
    void focusInEvent(QFocusEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    bool gestureEvent(QGestureEvent *event);
    void paintEvent(QPaintEvent *event) override;
    bool backEvent();                                   //点击返回按钮式由派生类做判断，要不要返回

private:
    QLabel *backgroundLabel;
    QPoint m_mouseSrcPos;   //滑动的初始点
    QPoint m_mouseDstPos;   //滑动的终点
    QLabel *total_label;    //图片滑动的动态页面/放大后的移动页面（大小变化）
    int curIndex;
    bool isFirstDouble;     //是否第一次双击   default:true
    bool isZoomMode;        //是否缩放模式    default:false
    QPixmap currentPixmap;

    int horizontalOffset; //[-60, 60]
    int verticalOffset;   //[-80, 80]
    qreal rotationAngle;
    qreal scaleFactor;//缩放比例
    qreal currentStepScaleFactor;

    void goNextImage();
    void goPrevImage();
    double getScaleValue(QSize img, QSize view);

private slots:
    void menuView(void);
    void albumMenuSlot(int idx);

private:
    void panTriggered(QPanGesture *gesture);
    void pinchTriggered(QPinchGesture *gesture);
    void swipeTriggered(QSwipeGesture *gesture);
    void moveImgHandler(int direction, int horiz, int verti);
    QPushButton *menuButton;
    QPushButton *backButton;
    QSize IMAGE_SIZE;
    QSize ITEM_SIZE;
    QStringList files;                      // 文件目录下所有的图像文件名
    int position;
};

#endif // AlbumWidget_H
