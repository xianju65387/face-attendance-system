#ifndef REGISTERWIN_H
#define REGISTERWIN_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QTimerEvent>

namespace Ui {
class RegisterWin;
}

class RegisterWin : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWin(QWidget *parent = nullptr);
    ~RegisterWin();


private slots:
    void on_resetBtn_clicked();

    void on_addpicBtn_clicked();

    void on_registerBtn_clicked();

    void on_videoswiitchBtn_clicked();

    void on_cameraBtn_clicked();

private:
    Ui::RegisterWin *ui;
    int timerid = 0;
    cv::VideoCapture cap;
    void timerEvent(QTimerEvent *e);
    cv::Mat image;
};

#endif // REGISTERWIN_H
