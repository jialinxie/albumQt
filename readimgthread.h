#ifndef READIMGTHREAD_H
#define READIMGTHREAD_H
#include <QThread>
class readImgThread : public QThread
{
    Q_OBJECT
public:
    readImgThread();
    virtual void run();
    void stop();
signals:
    void sendReadImgComplete();

private:

};

#endif // READIMGTHREAD_H
