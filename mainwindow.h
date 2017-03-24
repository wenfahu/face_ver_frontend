#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<opencv2/opencv.hpp>
#include<QFileDialog>
#include<QTextCodec>
#include<QTimer>
#include <sstream>

/*
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
*/

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/*
#include <future>
#include <queue>
*/

// #include <QDebug>
#include "socketthread.h"

using namespace std;
using namespace cv;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    Mat picture[2];
    Mat cvframe;
    QTimer *timer;
    QImage qimage;

    String face_cascade_name = "/home/wenfahu/faces/face_det/build/lbpcascade_frontalface.xml";
    String eyes_cascade_name = "/home/wenfahu/faces/face_det/build/haarcascade_eye_tree_eyeglasses.xml";
    CascadeClassifier eyes_cascade;
    CascadeClassifier face_cascade;
    // queue<future<bool> > responses;


    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QImage open_img(int flag);
    VideoCapture capture;
    void openCamera();   //打开摄像头

    void closeCamera();  //关闭摄像头
    void takingPictures();  //拍照
    // string saveImageQ(const QPixmap*);
    // string saveImageOCV(Mat);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    // void on_pushButton_3_clicked();


    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();
    void readFrame();    //读取当前帧信息
    void on_socket_result(const string &result);



private:
    Ui::MainWindow *ui;
    SocketThread *socketThread;
    bool hasSocketResult;
    string lastSocketResult;
};

#endif // MAINWINDOW_H
