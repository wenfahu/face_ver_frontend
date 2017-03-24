#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H
#include<opencv2/opencv.hpp>
#include <QThread>
#include <string>
#include <QMutex>
#include <QPixmap>
#include<zmq.hpp>
#include <functional>
using namespace cv;
using namespace std;


class SocketThread : public QThread
{
    Q_OBJECT
    void run();
// signals:
    // void resultReady(const string &result);

public:
    typedef std::function<void(const string &)> Callback;
    void init(const string &socketTarget, Callback callback);
    bool trySetData(const Mat &cvFrame, const QPixmap &cardPixmap);
    void stopLoop() { need_end = true; }
    void consumeResult();
    ~SocketThread() {
        this->QThread::~QThread();
        if (cvFrame != nullptr) {
            delete cvFrame;
            cvFrame = nullptr;
        }
        if (cardImg != nullptr) {
            delete cardImg;
            cardImg = nullptr;
        }
    }

private:
    string mTarget;
    string request_verification(zmq::socket_t &socket);
    void sendResult(const string& res);
    QMutex mInputMutex, mOutputMutex;
    Mat *cvFrame;
    Mat *cardImg;
    bool need_end;
    bool _inited;
    Callback mCallback;

};

#endif // SOCKETTHREAD_H
