#include "faceattendance.h"

#include <QApplication>
#include <opencv2/core.hpp>

int main(int argc, char *argv[])
{
    // Fix: this MinGW/GCC-built OpenCV 4.5.2 dispatches cvtColor() to its AVX2
    // code path, which crashes with an access violation (0xC0000005) inside
    // cv::hal_AVX2. setUseOptimized(false) makes OpenCV use the baseline SIMD
    // path instead, which does not crash.
    // Must be called before the first cv::cvtColor() call (timerEvent).
    cv::setUseOptimized(false);

    QApplication a(argc, argv);
    FaceAttendance w;
    w.show();
    return QApplication::exec();
}
