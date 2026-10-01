#ifndef ATTENDENCEWIN_H
#define ATTENDENCEWIN_H

#include "qfaceobject.h"

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <cstring>
#include <QSqlTableModel>
#include <QSqlRecord>

QT_BEGIN_NAMESPACE
namespace Ui {
class AttendenceWin;
}
QT_END_NAMESPACE

class AttendenceWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit AttendenceWin(QWidget *parent = nullptr);
    ~AttendenceWin() override;

signals:
    // 跨线程排队要求的参数不能带引用，否则 Qt 会报
    // "Cannot queue arguments of type 'cv::Mat&'"，信号发不出去
    void query(const cv::Mat &image);

private slots:
    void accept_client();
    void read_data();
    void recv_faceid(int64_t);

private:
    Ui::AttendenceWin *ui;
    QTcpServer mserver;
    QTcpSocket *msocket;
    quint64 bsize; //图片的总字节数

    QFaceObject fobj;
    QSqlTableModel model;

};
#endif // ATTENDENCEWIN_H
