#include "registerwin.h"
#include "ui_registerwin.h"
#include <QFileDialog>
#include "qfaceobject.h"
#include <QSqlTableModel>
#include <QSqlRecord>
#include <qmessagebox.h>

RegisterWin::RegisterWin(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegisterWin)
{
    ui->setupUi(this);
}

RegisterWin::~RegisterWin()
{
    delete ui;
}

void RegisterWin::timerEvent(QTimerEvent *e)
{
    //获取摄像头数据并且显示在界面上
    if(!cap.isOpened()) return;
    cap>>image;
    if(image.empty())  return;


    //显示在窗口上
    //mat --> QImage
    cv::Mat rgbMat;
    cv::cvtColor(image,rgbMat,cv::COLOR_BGR2RGB);
    QImage qImg(rgbMat.data,rgbMat.cols,rgbMat.rows,rgbMat.step1(),QImage::Format_RGB888);
    //在qt界面上显示
    QPixmap mmp = QPixmap::fromImage(qImg);
    mmp = mmp.scaledToWidth(ui->headpicLb->width());
    ui->headpicLb->setPixmap(mmp);

}

void RegisterWin::on_resetBtn_clicked()
{
    //清空数据
    ui->nameEdit->clear();
    ui->birthdayEdit->setDate(QDate(2000,1,1));
    ui->addressEdit->clear();
    ui->phoneEdit->clear();
    ui->picFileEdit->clear();

}


void RegisterWin::on_addpicBtn_clicked()
{
    //通过文化对话窗选中图片
    QString filepath = QFileDialog::getOpenFileName(this);
    ui->picFileEdit->setText(filepath);
    //显示图片
    QPixmap mmp(filepath);
    mmp = mmp.scaledToWidth(ui->headpicLb->width());
    ui->headpicLb->setPixmap(mmp);
}


void RegisterWin::on_registerBtn_clicked()
{
    //1.通过照片，结合faceobject模块得到faceid
    QFaceObject faceobj;
    cv::Mat image = cv::imread(ui->picFileEdit->text().toUtf8().data());

    if (image. empty ())
    {
        QMessageBox:: warning ( this , "提示" , "图片读取失败，请重新选择一张照片" ); return ; // ← 有这一句，imwrite 就永远不会收到空图 }
    }

    int faceID = faceobj.face_register(image);

    //把头像保存到当前目录下
    QString headfile = QString("./data/%1.jpg").arg(QString(ui->nameEdit->text().toUtf8().toBase64()));
    cv::imwrite(headfile.toUtf8().data(),image);

    //2.将个人信息传入数据库employee中
    //不用写 insert/update/select 语句，调用函数即可，上手简单，只适合**单张表**。
    QSqlTableModel model;
    model.setTable("employee");

    // 获取空记录模板：拥有employee表全部字段，数据为空
    QSqlRecord record = model.record();
    //设置数据
    record.setValue("name",ui->nameEdit->text());
    record.setValue("sex",ui->mrb->isChecked()?"男":"女");
    record.setValue("birthday",ui->birthdayEdit->text());
    record.setValue("address",ui->addressEdit->text());
    record.setValue("phone",ui->phoneEdit->text());
    record.setValue("faceID",faceID);
    //头像路径
    record.setValue("headfile",headfile);
    //把记录插入到数据库表
    bool ret = model.insertRecord(-1,record);

    //3.提示注册成功
    if(ret)
    {
        //提交
        model.submitAll();
        QMessageBox::information(this,"注册提示","注册成功!");

    }
    else
    {
        QMessageBox::information(this,"注册提示","注册失败!");
    }

}


void RegisterWin::on_videoswiitchBtn_clicked()
{
    if(ui->videoswiitchBtn->text() == "打开摄像头")
    {
        //打开摄像头
        if(cap.open(0,cv::CAP_DSHOW))
        {
            ui->videoswiitchBtn->setText("关闭摄像头");
            //启动定时器事件
            timerid = startTimer(100);
        }

    }
    else
    {
        //先停定时器并清零，再释放摄像头
        if(timerid)
        {
            killTimer(timerid);
            timerid = 0;
        }
        cap.release();
        ui->videoswiitchBtn->setText("打开摄像头");
    }

}


void RegisterWin::on_cameraBtn_clicked()
{
    //保存数据
    QString headfile = QString("./data/%1.jpg").arg(QString(ui->nameEdit->text().toUtf8().toBase64()));
    ui->picFileEdit->setText(headfile);
    cv::imwrite(headfile.toUtf8().data(),image);

    if(timerid)
    {
        killTimer(timerid);
        timerid = 0;
    }
    cap.release();
    ui->videoswiitchBtn->setText("打开摄像头");
}

