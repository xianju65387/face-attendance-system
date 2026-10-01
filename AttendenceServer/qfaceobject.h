#ifndef QFACEOBJECT_H
#define QFACEOBJECT_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <seeta/FaceEngine.h>

//人脸数据库存储 ，人脸识别 ，人脸检测
class QFaceObject : public QObject
{
    Q_OBJECT
public:
    explicit QFaceObject(QObject *parent = nullptr);
    ~QFaceObject();

signals:
    void send_faceid(int64_t faceid);
public slots:
    int64_t face_register(cv::Mat& faceImage);
    int face_query(const cv::Mat &faceImage);

private:
    seeta::FaceEngine *fengineptr;
};

#endif // QFACEOBJECT_H
