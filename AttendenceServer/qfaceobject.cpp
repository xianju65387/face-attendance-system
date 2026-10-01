#include "qfaceobject.h"

QFaceObject::QFaceObject(QObject *parent)
    : QObject{parent}
{
    //初始化引擎
    seeta::ModelSetting FDmode("E:/SeetaFace2-master/SeetaFace2-master/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install/bin/modle/fd_2_00.dat",seeta::ModelSetting::CPU,0);
    seeta::ModelSetting PDmode("E:/SeetaFace2-master/SeetaFace2-master/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install/bin/modle/pd_2_00_pts5.dat",seeta::ModelSetting::CPU,0);
    seeta::ModelSetting FRmode("E:/SeetaFace2-master/SeetaFace2-master/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install/bin/modle/fr_2_10.dat",seeta::ModelSetting::CPU,0);

    this->fengineptr = new seeta::FaceEngine(FDmode,PDmode,FRmode);
    //导入一有的人脸数据库
    this->fengineptr->Load("./face.db");
}

QFaceObject::~QFaceObject()
{
    delete fengineptr;
}

//实现人脸的注册
int64_t QFaceObject::face_register(cv::Mat &faceImage)
{
    //把opencv的Mat数据转为Seetaface的数据
    SeetaImageData simage;
    simage.data = faceImage.data;
    simage.width = faceImage.cols;
    simage.height = faceImage.rows;
    simage.channels = faceImage.channels();

    //`Register`：提取特征 → **存入内存库**，返回新 faceid（录入人脸）
    int64_t faceid = this->fengineptr->Register(simage);    //注册返回人脸id

    //存在的用户再次注册不会产生新的用户
    if(faceid >= 0)
    {
        fengineptr->Save("./face.db");
    }

    return faceid;
}

//实现人脸的查询
int QFaceObject::face_query(const cv::Mat &faceImage)
{
    SeetaImageData simage;
    simage.data = faceImage.data;
    simage.width = faceImage.cols;
    simage.height = faceImage.rows;
    simage.channels = faceImage.channels();

    float similarity = 0;

    //`Query`：提取特征 → **遍历库比对**，返回匹配 faceid + 相似度（识别考勤）
    int64_t faceid = fengineptr->Query(simage,&similarity); //运行时间比较长
    if(similarity > 0.7)
    {
        emit send_faceid(faceid);
    }
    else
    {
        emit send_faceid(-1);
    }

    return faceid;
}
