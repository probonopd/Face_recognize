#ifndef UTILS_H
#define UTILS_H

#include <QImage>
#include <opencv2/opencv.hpp>

QImage Mat2QImage(cv::Mat const& src);
cv::Mat QImage2Mat(QImage const& src);

#endif // UTILS_H
