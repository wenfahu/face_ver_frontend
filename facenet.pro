#-------------------------------------------------
#
# Project created by QtCreator 2017-03-07T10:25:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = facenet
TEMPLATE = app
QMAKE_CC = /usr/local/bin/gcc
QMAKE_CXX = /usr/local/bin/g++

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
INCLUDEPATH += /usr/local/include/opencv \
/usr/local/include/opencv2  \



LIBS +=  -L/usr/local/lib/ -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lzmq \
 -lboost_filesystem -lboost_system -lopencv_objdetect
