#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <functional>

// namespace fs = boost::filesystem;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    socketThread = Q_NULLPTR;
    ui->setupUi(this);
    timer = new QTimer(this);;
    if( !face_cascade.load( face_cascade_name ) ){
        printf("--(!)Error loading face cascade\n");
    };
    if( !eyes_cascade.load( eyes_cascade_name ) ){
        printf("--(!)Error loading eyes cascade\n"); ; };


    connect(timer, SIGNAL(timeout()), this, SLOT(readFrame()));
    // connect(timer, SIGNAL(timeout()), this, SLOT(updateLabel()));

}

MainWindow::~MainWindow()
{
    delete ui;
}


 QImage MainWindow::open_img(int flag)
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"),"",tr("Image File(*.bmp *.jpg *.jpeg *.png)"));
    QTextCodec *code = QTextCodec::codecForName("gb18030");
    std::string name = code->fromUnicode(filename).data();
    if(name.length()>1)
    {
        picture[flag] = imread(name, 1);
        cvtColor(picture[flag], picture[flag], CV_BGR2RGB);
        QImage img = QImage((const unsigned char *)(picture[flag].data), picture[flag].cols, picture[flag].rows,
                picture[flag].cols*picture[flag].channels(), QImage::Format_RGB888);
        return img;

  }

}


void MainWindow::on_pushButton_clicked()
{
    QImage img = open_img(0);
    ui->label_1->clear();
    if(img.height()>img.width())
   {
        img = img.scaled (img.width()*ui->label_1->height()/img.height(),ui->label_1->height());
   }
   else
   {
        img = img.scaled (ui->label_1->width(),img.height()*ui->label_1->width()/img.width());
   }

    ui->label_1->setPixmap(QPixmap::fromImage(img));
    //ui->label_1->resize(ui->label_1->pixmap()->size());
}

void MainWindow::on_pushButton_2_clicked()
{
    QImage img=open_img(1);
    if(img.height()>img.width())
   {
        img = img.scaled (img.width()*ui->label_2->height()/img.height(),ui->label_2->height());
   }
   else
   {
        img = img.scaled (ui->label_2->width(),img.height()*ui->label_2->width()/img.width());
   }

    ui->label_2->clear();
    ui->label_2->setPixmap(QPixmap::fromImage(img));
}

/*
string MainWindow::saveImageQ(const QPixmap *pmap){
    boost::uuids::random_generator generator;
    boost::uuids::uuid uuid = generator();
    stringstream ss;
    ss << uuid << ".png";
    boost::filesystem::path curr_path( boost::filesystem::current_path() );
    fs::path image_folder ("images");
    fs::path image_path = curr_path / image_folder;
    if (!fs::exists(image_path)){
        boost::system::error_code returnedError;
        boost::filesystem::create_directories( image_path, returnedError );
    }
    fs::path img_path = image_path / fs::path(ss.str());
    QFile image(QString::fromStdString(img_path.string()));// /home/wenfahu/new_face/facenet/trained_models/20170216-091149.zip

    image.open(QIODevice::WriteOnly);
    pmap->save(&image, "PNG");
    return img_path.string();
}

string MainWindow::saveImageOCV(Mat img){
    return "";
    boost::uuids::random_generator generator;
    boost::uuids::uuid uuid = generator();
    stringstream ss;
    ss << uuid << ".png";
    boost::filesystem::path curr_path( boost::filesystem::current_path() );
    fs::path image_folder ("images");
    fs::path image_path = curr_path / image_folder;
    if (!fs::exists(image_path)){
        boost::system::error_code returnedError;
        boost::filesystem::create_directories( image_path, returnedError );
    }
    fs::path img_path = image_path / fs::path(ss.str());
    imwrite(img_path.string(), img);
    return img_path.string();
}
*/

/*
void MainWindow::on_pushButton_3_clicked()
{
    const QPixmap* pixmap1 = ui->label_1->pixmap();
    const QPixmap* pixmap2 = ui->label_2->pixmap();
    boost::uuids::random_generator generator;
    boost::uuids::uuid uuid1 = generator();
    stringstream ss1;
    ss1 << uuid1 << ".png";
    boost::uuids::uuid uuid2 = generator();
    stringstream ss2;
    ss2 << uuid2 << ".png";
    boost::filesystem::path curr_path( boost::filesystem::current_path() );
    fs::path image_folder ("images");
    fs::path image_path = curr_path / image_folder;
    if (!fs::exists(image_path)){
        boost::system::error_code returnedError;
        boost::filesystem::create_directories( image_path, returnedError );
    }
    fs::path image1_path = image_path / fs::path(ss1.str());
    fs::path image2_path = image_path / fs::path(ss2.str());
    QFile image1(QString::fromStdString(image1_path.string())),
            image2(QString::fromStdString(image2_path.string()));
    image1.open(QIODevice::WriteOnly);
    image2.open(QIODevice::WriteOnly);
    pixmap1->save(&image1, "PNG");
    pixmap2->save(&image2, "PNG");
    json j;
    j["cam"] = image1_path.string();
    j["id"] = image2_path.string();
    string msg_str;
    msg_str = j.dump();
    json meta = json::meta();
    string temp2 = meta.dump();
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);
    socket.connect ("tcp://localhost:5555");
    zmq::message_t request (msg_str.size());
    memcpy (request.data (), (msg_str.c_str()), (msg_str.size()));
    std::cout<<msg_str.size()<<std::endl;
    socket.send(request);
    zmq::message_t reply;
    socket.recv(&reply);
    std::string msg_str2(static_cast<char*>(reply.data()), reply.size());
    json res = json::parse(msg_str2);
    bool result = res["acc"];
    if (result) {
        ui->verification_result->setText(QString("same"));
    } else {
        ui->verification_result->setText(QString("diff"));
    }
}
*/

void MainWindow::on_socket_result(const string &result) {
    this->lastSocketResult = result;
    this->hasSocketResult = true;
}

void MainWindow::openCamera()
{
    capture = cv::VideoCapture(0);

    if (socketThread == Q_NULLPTR) {
        socketThread = new SocketThread();
    }
    socketThread->init("", std::bind(&MainWindow::on_socket_result, this, std::placeholders::_1));
    this->hasSocketResult = false;
    this->lastSocketResult = "";
    socketThread->start();

    timer->start(33);
}


//读取摄像头信息
void MainWindow::readFrame()
{
    //捕获摄像头一帧图像
    capture >> cvframe;
    if (this->hasSocketResult) {
        this->hasSocketResult = false;
        socketThread->consumeResult();
        // TODO: consume result
        const string a = this->lastSocketResult;
        QLabel *label = this->ui->verification_result;
        if("acc" == a){
            label->setText("True");
            QFont font = label->font();
            font.setPointSize(72);
            font.setBold(true);
            label->setFont(font);

        } else {
            label->setText("False");
            QFont font = label->font();
            font.setPointSize(72);
            font.setBold(true);
            label->setFont(font);
        }
    }

    // detect faces
    vector<cv::Rect> faces; //, filtered_faces;
    Mat frame_gray;
    cvtColor(cvframe, frame_gray, COLOR_BGR2GRAY );
    equalizeHist(frame_gray, frame_gray);

    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0, Size(80, 80));
    for( size_t i = 0; i < faces.size(); i++ )
    {
        Mat faceROI = frame_gray( faces[i] );
        std::vector<Rect> eyes;

        //-- In each face, detect eyes
        eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30) );
        if( eyes.size() == 2)
        {
            // filtered_faces.push_back(faces[i]);
            if(this->ui->label_2->pixmap()){
                socketThread->trySetData(cvframe, *(ui->label_2->pixmap()));
            }

            Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
            ellipse( cvframe, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 0 ), 2, 8, 0 );
        }
    }
    //updateLabel();
    //颜色通道转换
    cv::cvtColor(cvframe, cvframe, CV_BGR2RGB);

    //获取QImage格式的图像
    qimage = QImage((const uchar*)(cvframe.data), cvframe.cols, cvframe.rows, QImage::Format_RGB888);

    //将图像显示在label中， 之前要把QImage格式的图像转换成QPixmap格式
    ui->label_1->setPixmap(QPixmap::fromImage(qimage));
}

//拍照
void MainWindow::takingPictures()
{
    ui->label_1->setPixmap(QPixmap::fromImage(qimage));
}

//关闭摄像头

void MainWindow::closeCamera()
{
    timer->stop();
    socketThread->stopLoop();
    socketThread->wait();
    socketThread = Q_NULLPTR;

    capture.release();
}

void MainWindow::on_pushButton_4_clicked()
{
    openCamera();

}

void MainWindow::on_pushButton_5_clicked()
{
    takingPictures();
    closeCamera();
}


