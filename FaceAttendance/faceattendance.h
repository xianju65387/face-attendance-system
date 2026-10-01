#ifndef FACEATTENDANCE_H
#define FACEATTENDANCE_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QTcpSocket>
#include <QTimer>

using namespace std;
using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui {
class FaceAttendance;
}
QT_END_NAMESPACE

class FaceAttendance : public QMainWindow
{
    Q_OBJECT

public:
    explicit FaceAttendance(QWidget *parent = nullptr);
    ~FaceAttendance() override;
    //定时器事件
    void timerEvent(QTimerEvent *e);

private slots:
    void timer_connect();
    void stop_connect();
    void start_connect();
    void recv_data();


private:

    Ui::FaceAttendance *ui;

    //摄像头
    cv::VideoCapture cap;

    //haar -- 级联分类器
    cv::CascadeClassifier cascade;

    //创建一个网络套接字 定时器
    QTcpSocket msocket;
    QTimer mtimer;

    //标志是否是同一个人的人脸进入到识别区域
    int flag;

    //保存人脸的数据
    cv::Mat faceMat;

};
#endif // FACEATTENDANCE_H
