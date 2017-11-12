#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList args = QCoreApplication::arguments();
    timer = new QTimer(this);

    string video = args.at(1).toStdString();
    if(video.size() > 1)
    {
        cap = cv::VideoCapture(video);
        std::cout << "Reading frame from video!" <<std::endl;
    }
    else
    {
        cap = cv::VideoCapture(std::stoi(video));
        std::cout << "Reading frame from camera!" << std::endl;
    }
    if(!this->cap.isOpened())
        QMessageBox::warning(this, "Warning", "Can't open camera!");

    std::string indexf = args.at(2).toStdString();   // index file
    this->fg = new FeatureGroup(indexf, &this->sf);

    timer->start(33);
    connect(timer, SIGNAL(timeout()), this, SLOT(grabFace()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    this->close();
}

void MainWindow::showTopK(std::vector<Feature> result)
{
    QStringList args = QCoreApplication::arguments();


    std::string headpath = args.at(3).toStdString(); // icon folder
    QSize s = ui->face1->size();

    //face 1
    QImage top1(QString::fromStdString(headpath + "/" + result[0].filename));
    ui->face1->setPixmap(QPixmap::fromImage(top1.scaled(s, Qt::KeepAspectRatio)));
    int len = result[0].filename.size();
    string name = result[0].filename.substr(2, len-6);

    ui->name->setText(QString::fromStdString(name));
    ui->semb->setText(QString::number(result[0].similarity_with_goal));

//    //face2
//    QImage top2(QString::fromStdString(headpath + result[1].filename));
//    ui->face2->setPixmap(QPixmap::fromImage(top2.scaled(s, Qt::KeepAspectRatio)));
//    ui->semb2->setText(QString::number(result[1].similarity_with_goal));

//    //face3
//    QImage top3(QString::fromStdString(headpath + result[2].filename));
//    ui->face3->setPixmap(QPixmap::fromImage(top3.scaled(s, Qt::KeepAspectRatio)));
//    ui->semb3->setText(QString::number(result[2].similarity_with_goal));

//    //face4
//    QImage top4(QString::fromStdString(headpath + result[3].filename));
//    ui->face4->setPixmap(QPixmap::fromImage(top4.scaled(s, Qt::KeepAspectRatio)));
//    ui->semb4->setText(QString::number(result[3].similarity_with_goal));

//    //face5
//    QImage top5(QString::fromStdString(headpath + result[4].filename));
//    ui->face5->setPixmap(QPixmap::fromImage(top5.scaled(s, Qt::KeepAspectRatio)));
//    ui->semb5->setText(QString::number(result[4].similarity_with_goal));
}

void MainWindow::grabFace()
{
    if(1)
    {
        cv::Mat src;
        this->cap >> src;
        cv::resize(src, this->frame, cv::Size(640,360));
    }
    else
        this->cap >> this->frame;
    float feat[2048];
    cv::Rect face_rect;
    std::vector<Feature> result;

    cv::Rect rec(0,0,640,240);   //this setting is aimed at video.mp4
    if(sf.GetFeature(this->frame(rec), feat))
    //if(sf.GetFeature(this->frame, feat))
    {
         face_rect.x = sf.bbox[0].x;
         face_rect.y = sf.bbox[0].y;
         face_rect.width = sf.bbox[0].width;
         face_rect.height = sf.bbox[0].height;
         cv::rectangle(this->frame, face_rect, CV_RGB(0, 255, 0), 2, 8, 0);
         sf.bbox.clear();

         this->fg->FindTopK(TPK, feat, result);

         this->showTopK(result);
    }
    QImage img = Mat2QImage(this->frame);
    ui->video->setPixmap(QPixmap::fromImage(img));
}
