#include "faceattendance.h"
#include "ui_faceattendance.h"
#include <QDebug>
#include <QImage>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
FaceAttendance::FaceAttendance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FaceAttendance)
{
    ui->setupUi(this);
    //打开摄像头

    cap.open(0, CAP_DSHOW);

    //启动定时器事件
    startTimer(100);
    //导入级联分类器文件
    cascade.load("E:/OpenCV-4.5.2/opencv-4.5.2/opencv-4.5.2/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install/etc/haarcascades/haarcascade_frontalface_alt2.xml");

    //QTcpsocket当断开链接是会发送disconnected信号 链接成功时会发送connected信号
    connect(&msocket,&QTcpSocket::disconnected,this,&FaceAttendance::start_connect);
    connect(&msocket,&QTcpSocket::connected,this,&FaceAttendance::stop_connect);

    //关联接收数据的槽函数
    connect(&msocket,&QTcpSocket::readyRead,this,&FaceAttendance::recv_data);

    //定时链接服务器
    connect(&mtimer,&QTimer::timeout,this,&FaceAttendance::timer_connect);

    //启动定时器 每5s链接一次 直到链接成功就不再链接
    mtimer.start(5000);
    flag = 0;

    ui->widgetLb->hide();
}



FaceAttendance::~FaceAttendance()
{
    delete ui;
}

void FaceAttendance::timerEvent(QTimerEvent *e)
{
    //采集数据
    Mat srcImage;
    if(cap.grab())
    {
        cap.read(srcImage); //读取一帧数据

    }

    //把图片大小设与显示窗口一样大
    //cv::resize(srcImage,srcImage,Size(480,480));

    Mat grayImage;
    //转灰度图 灰度图处理更快
    cvtColor(srcImage,grayImage,COLOR_BGR2GRAY);

    //检测人脸数据
    std::vector<Rect> faceRects;
    //使用级联分类器，在灰度图`grayImage`里面搜索人脸；找到的每一张人脸的位置、大小，保存到`faceRects`容器。
    cascade.detectMultiScale(grayImage,faceRects);//检测人脸
    if(faceRects.size()>0 && flag>=0)
    {

        Rect rect = faceRects.at(0);//第一个人脸的矩形框
        //rectangle(srcImage,rect,Scalar(0,0,255));

        //移动标签框
        ui->headpicLb->move(rect.x,rect.y);

        if(flag > 2){

            if (msocket. state () != QAbstractSocket::ConnectedState)
            { // 没连上：这一帧不发，也不把 flag 置成 -2， // 下一帧（100ms 后）会再走到这里继续尝试
            }
            else
            {
            //发送人脸数据 将Mat数据转化为QbyteArray  -->编码成jpg格式再发送
            vector<uchar> buf;

            // 把Mat压缩编码成jpg二进制，存入vector
            cv::imencode(".jpg",srcImage,buf);

            // vector转QByteArray
            // (const char*)buf.data() 将 uchar类型的buf强转为数组中的类型 .data()用来取首地址
            QByteArray byte((const char*)buf.data(),buf.size());

            //准备发送 1.文件大小 2.文件数据

            //`byte`是 jpg 图片二进制的 QByteArray。
            //backsize`保存图片字节总数，作为包头，告诉服务端后面图片有多少字节。
            quint64 backsize = byte.size();

            //最终打包好要发送的完整数据包，是个字节容器
            QByteArray sendData;

            //创建序列化流`stream`，写入的数据全部放进 sendData 里面。
            QDataStream stream (&sendData,QIODevice::WriteOnly);
            stream.setVersion(QDataStream::Qt_6_0);

            //写数据 先写入字节长度 再写入byte(图片的二进制)
            stream<<backsize<<byte;

            //打包发送数据
            msocket.write(sendData);
            flag = -2;

            faceMat = srcImage(rect);
            //保存
            cv::imwrite("./face.jpg",faceMat);

            }
        }
        flag++;
    }
    if(faceRects.size() == 0)
    {
        //把人脸框移动到中心位置
        ui->headpicLb->move(100,70);
        ui->widgetLb->hide();
        flag = 0;
    }

    if(srcImage.data == nullptr )
    {
        return ;
    }
    //把opencv中的mat格式（bgr） 转成 QImage（rgb）
    cvtColor(srcImage,srcImage,COLOR_BGR2RGB);
    QImage image(srcImage.data,srcImage.cols,srcImage.rows,srcImage.step1(),QImage::Format_RGB888);
    QPixmap mmp = QPixmap::fromImage(image);

    ui->videoLb->setPixmap(mmp);
}

void FaceAttendance::timer_connect()
{
    //链接服务器
    msocket.connectToHost("192.168.88.1",9000);
    qDebug() << "正在连接服务器";
}

void FaceAttendance::stop_connect()
{
    mtimer.stop();
    qDebug() << "成功连接服务器";
}

void FaceAttendance::start_connect()
{
    mtimer.start(5000); //启动定时器
    qDebug() << "断开链接";
}

void FaceAttendance::recv_data()
{
    QByteArray array = msocket.readAll();
    //解析JSON数据
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(array,&err);
    if(err.error != QJsonParseError::NoError)
    {
        qDebug() << "json数据错误";
        return ;
    }
    QJsonObject obj = doc.object();
    QString employeeID = obj.value("employeeID").toVariant().toString();
    QString name = obj.value("name").toString();
    QString department = obj.value("department").toString();
    QString timestr = obj.value("time").toString();

    ui->numberEdit->setText(employeeID);
    ui->nameEdit->setText(name);
    ui->departmentEdit->setText(department);
    ui->timeEdit->setText(timestr);

    //显示头像
    ui->headLb->setStyleSheet("border-radius:75px;border-image: url(./face.jpg);");

    ui->widgetLb->show();

}
