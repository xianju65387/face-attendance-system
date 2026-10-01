#include "attendencewin.h"
#include "ui_attendencewin.h"
#include <opencv2/opencv.hpp>
#include <QDateTime>
#include <QThread>
#include <qsqlquery.h>

AttendenceWin::AttendenceWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AttendenceWin)
{
    ui->setupUi(this);
    //qtcpserver当有客户端链接newconnection
    connect(&mserver,&QTcpServer::newConnection,this,&AttendenceWin::accept_client);

    //监听 启动服务器；失败（例如端口被别的程序占用）必须看得见，否则会静默失效
    if(!mserver.listen(QHostAddress::Any,9000))
        qDebug() << "监听 9000 端口失败:" << mserver.errorString();
    bsize = 0;

    //给Sql模型绑定表格
    model.setTable("employee");

    //创建一个线程
    QThread *thread = new QThread();

    //把QFaceObject对象移动到thread线程中执行
    fobj.moveToThread(thread);
    thread->start();

    connect(this,&AttendenceWin::query,&fobj,&QFaceObject::face_query);

    //关联QFaceOBject对象里面的send_faceid信号
    connect(&fobj,&QFaceObject::send_faceid,this,&AttendenceWin::recv_faceid);
}

AttendenceWin::~AttendenceWin()
{
    delete ui;
}

//接受客户端链接
void AttendenceWin::accept_client()
{
    //取出客户端中的套接字 赋给msocket
    msocket = mserver.nextPendingConnection();
    //当客户端有数据到达会发送readyread信号  **操作系统收到对方发来的数据，放到 socket 接收缓冲区里，Qt 就自动发出 readyRead 信号**。
    connect(msocket,&QTcpSocket::readyRead,this,&AttendenceWin::read_data);
}

//读取客户端发送的数据
void AttendenceWin::read_data()
{
    //读取所有的数据
    QDataStream stream(msocket);//把套接字绑定到流中
    stream.setVersion(QDataStream::Qt_6_0);
    if(bsize == 0)
    {
        if(msocket->bytesAvailable() < (qint64)sizeof(bsize))
        {
            return;
        }
        else
        {
            //采集数据长度 将读取到的数据头写入bsize 直接拿走
            //**读取操作会直接把这 8 字节从 socket 内核缓冲区拿走、消耗掉**，所以缓冲区里就少了 8 字节。
            stream >> bsize;
        }
    }

    //数据如果没有发送完
    if(msocket->bytesAvailable () < bsize)
    //当前 socket 缓冲区剩下的图片字节数量 ＜ `bsize`（图片总共需要的字节数）
    // → 图片还没收完整，直接 return，等下一轮网络数据到达触发 readyRead。
    {
        return;
    }
    else
    {
        QByteArray data;
        stream >> data;
        bsize = 0;
        if(data.size() == 0)
        {
            return;
        }
        //显示图片
        QPixmap mmp;
        //降jpg文件的二进制加载成QPixmap
        mmp.loadFromData(data,"jpg");
        mmp = mmp.scaled(ui->picLb->size()); // 将图片缩放到label的大小
        ui->picLb->setPixmap(mmp);


        //识别人脸
        cv::Mat faceImage;
        std::vector<uchar> decode;
        decode.resize(data.size());
        memcpy(decode.data(),data.data(),data.size());
        faceImage = cv::imdecode(decode,cv::IMREAD_COLOR);
        //int faceid = fobj.face_query(faceImage);//消耗资源较多

        emit query(faceImage);
    }

}

void AttendenceWin::recv_faceid(int64_t faceid)
{
    qDebug() <<"识别到的人脸:"<< faceid;
    //从数据库中查询faceid对应的个人信息
    //给模型设置过滤器
    if(faceid < 0)
    {
        QString sdmsg = QString(R"({"employeeID":"-1","name":"","department":"","time":"%1"})")
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
        msocket->write(sdmsg.toUtf8()); //打包数据发送给客户端
        return ;
    }
    model.setFilter(QString("faceID=%1").arg(faceid));
    //查询
    model.select();
    //判断实付查询到数据
    if(model.rowCount() == 1)
    {
        //工号 姓名 部门 时间
        //{employeeID:%1,name:%2,department:软件,time:%3}
        QSqlRecord record = model.record(0);
        QString sdmsg = QString(R"({"employeeID":"%1","name":"%2","department":"软件","time":"%3"})")
                            .arg(record.value("employeeID").toString()).arg(record.value("name").toString())
                            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

        //把数据写入到考勤表
        QString insertSql = QString("insert into attendance(employeeID) values(%1)").arg(record.value("employeeID").toString());
        QSqlQuery query;
        if(!query.exec(insertSql))
        {
            QString sdmsg = QString(R"({"employeeID":"-1","name":"","department":"","time":"%1"})")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
            msocket->write(sdmsg.toUtf8()); //打包数据发送给客户端
            return;
        }
        else
        {
            msocket->write(sdmsg.toUtf8()); //打包数据发送给客户端
        }


    }

}
