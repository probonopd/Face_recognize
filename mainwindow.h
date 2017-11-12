#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define TPK 1

#include "SeetaFace.h"
#include "utils.h"

#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <opencv2/opencv.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void run();

public:
    void showTopK(std::vector<Feature> result);

private slots:
    void on_pushButton_clicked();
    void grabFace();

private:
    Ui::MainWindow *ui;

    QTimer *timer;
    cv::VideoCapture cap;
    cv::Mat frame;

    SeetaFace sf;
    FeatureGroup *fg;
};

#endif // MAINWINDOW_H
