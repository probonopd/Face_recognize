#include "utils.h"

QImage Mat2QImage(const cv::Mat &src)
{
    cv::Mat temp;
    cv::cvtColor(src, temp, CV_BGR2RGB);
    QImage dst = QImage((const uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dst.bits();
    return dst;
}

cv::Mat QImage2Mat(const QImage &src)
{
    cv::Mat tmp(src.height(), src.width(), CV_8UC3, (uchar*)src.bits(), src.bytesPerLine());
    cv::Mat result;
    cv::cvtColor(tmp, result, CV_BGR2RGB);
    return result;
}
