#include "socketthread.h"
#include <QMutexLocker>

void SocketThread::init(const string &socketTarget, Callback callback) {
    if (!this->_inited) {
        this->cvFrame = nullptr;
        this->cardImg = nullptr;
        this->_inited = true;
    }
    this->need_end = false;
    this->mTarget = socketTarget.empty() ? "tcp://localhost:5555" : socketTarget;
    this->mInputMutex.lock();
    this->mCallback = callback;
}

void SocketThread::run()
{
    zmq::context_t context(1);
    zmq::socket_t socket (context, ZMQ_REQ);
    socket.connect (this->mTarget);
    for (; !this->need_end; ) {
        string result = request_verification(socket);
        sendResult(result);
    }
}

bool SocketThread::trySetData(const Mat &cvFrame, const QPixmap &cardPixmap) {
    if (!this->mOutputMutex.tryLock()) {
        return false;
    }
    if (this->cvFrame == NULL) {
        this->cvFrame = new Mat(cvFrame.rows, cvFrame.cols, cvFrame.type());
    }
    if (cardImg == NULL) {
        cardImg = new Mat(cardPixmap.height(), cardPixmap.width(), CV_8UC3);
    }
    cvFrame.copyTo(*this->cvFrame);
    QImage swapped = cardPixmap.toImage();
    if ( swapped.format() == QImage::Format_RGB32 )
    {
       swapped = swapped.convertToFormat( QImage::Format_RGB888 );
    }
    cv::Mat( swapped.height(), swapped.width(),
        CV_8UC3,
        const_cast<uchar*>(swapped.bits()),
        static_cast<size_t>(swapped.bytesPerLine())
    ).copyTo(*cardImg);

    this->mInputMutex.unlock();
    return true;
}


string SocketThread::request_verification(zmq::socket_t &socket) {
    QMutexLocker _locker(&(this->mInputMutex));
    // TODO: make msg_str
    int frameSize = cvFrame->dataend - cvFrame->data;
    int cardSize = cardImg->dataend - cardImg->data;
    int total = 6 * sizeof(int) + frameSize + cardSize;

    zmq::message_t request(total);
    int *data = static_cast<int*>(request.data());
    int i = 0;
    data[i++] = frameSize;
    data[i++] = cvFrame->rows;
    data[i++] = cvFrame->cols;
    std::cout<<frameSize<<std::endl;
    std::cout<<cvFrame->rows<<" "<<cvFrame->cols<<std::endl;
    data[i++] = cardSize;
    data[i++] = cardImg->rows;
    data[i++] = cardImg->cols;
    std::cout<<cardSize<<std::endl;
    std::cout<<cardImg->rows<<" "<<cardImg->cols<<std::endl;
    std::cout<<data<<std::endl;
    memcpy(data + i, cvFrame->data, frameSize);
    memcpy(((char *)(data + i)) + frameSize, cardImg->data, cardSize);

    socket.send(request);
    socket.recv(&request);
    return std::string(static_cast<char*>(request.data()), request.size());
}

void SocketThread::sendResult(const string& res) {
    mCallback(res);
    QMutexLocker _locker(&(this->mOutputMutex));
}

void SocketThread::consumeResult() {
    mInputMutex.lock();
    mOutputMutex.unlock();
}
