#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 3 arguments: saving features of images in given folder into a file
    // argv[1]: images folder
    // argv[2]: name of saved file
    //default: /home/chenqi/chenqi/c++/Qt/Face/photo 71.index
    if(argc == 3)
    {
        string path = argv[1];
        string name = argv[2];
        saveAllFeature(path, name);
        return 0;
    }
    // 4 arguments: video/cam   .index file    icon folder
    //default: video.mp4 71.index /home/chenqi/chenqi/c++/Qt/Face/image/photo_icon
    else if(argc == 4)
    {
        MainWindow w;
        w.show();
        return a.exec();
    }
}
