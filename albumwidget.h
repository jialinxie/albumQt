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
#include "readimgthread.h"

#ifdef QT_PC
    #define ALBUM_PATH      "../Camera/"
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
class AlbumWidget : public QWidget
{
    Q_OBJECT
public:
    static AlbumWidget* getInstance(QWidget *parent){
        static AlbumWidget *m_instance = new AlbumWidget(parent);
        return m_instance;
    }
    int updateUI();

private:
    AlbumWidget(QWidget *parent = 0);
    ~AlbumWidget();
    void grabGestures(const QList<Qt::GestureType> &gestures);

protected:
    void focusInEvent(QFocusEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    bool gestureEvent(QGestureEvent *event);
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool backEvent();                                   //点击返回按钮式由派生类做判断，要不要返回

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

    QImage prevImage, nextImage;
    QImage currentImage;

    qreal horizontalOffset;
    qreal verticalOffset;
    qreal rotationAngle;
    qreal scaleFactor;
    qreal currentStepScaleFactor;

    void setLabelMove(bool enable);
    void goNextImage();
    void goPrevImage();
    void goToImage(int index);
    double getScaleValue(QSize img, QSize view);
    void updateBufferByKey(bool keyWay);
//    void updateBufferByMouse()

private slots:
    void slot_itemClicked(QListWidgetItem*);
    void menuView(void);
    void back2Album(void);
    void albumMenuSlot(int idx);
//    void readImgCompleteSlot(void);

private:
    QImage loadImage(const QString &fileName);
    void panTriggered(QPanGesture *gesture);
    void pinchTriggered(QPinchGesture *gesture);
    void swipeTriggered(QSwipeGesture *gesture);

    QLabel *mShowWidget;                    // 图像显示窗口
    QPushButton *menuButton;
    QPushButton *backButton;
    QPixmap cenPixmap;
    QPixmap showPixmap;
    QImage  cenImg;
    picListShow *pListShow;
    QDir dir;
    QStringList filters;
    QSize IMAGE_SIZE;
    QSize ITEM_SIZE;
    QString path;
    QStringList files;                      // 文件目录下所有的图像文件名
    int position;
};

class picListShow : public QListWidget{
    Q_OBJECT
public:
    explicit picListShow(QWidget *parent);
    ~picListShow();
    bool MoveStatus();
    bool setMoveStatus();
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void focusInEvent(QFocusEvent *event);
private:
    int yPos0;
    int yPos1;
    int xPos0;
    int xPos1;
    bool isMove;
    QPoint slidePoint;
    readImgThread thread0;

private slots:
    void readImgCompleteSlot(void);

};

#endif // AlbumWidget_H
