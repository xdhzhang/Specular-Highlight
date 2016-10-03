#-------------------------------------------------
#
# Project created by QtCreator 2016-10-03T11:28:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Highlight
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Basic/basic.cpp \
    QHighLight/qhighlight.cpp \
    QJointBilateralFilter/qjbf.cpp \
    QMat/qmat.cpp

HEADERS  += mainwindow.h \
    Basic/basic.h \
    QHighLight/qhighlight.h \
    QJointBilateralFilter/qjbf.h \
    QMat/qmat.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:\\opencv\\build\\install\\include

LIBS += -LC:\\opencv\\build\\install\\x64\\vc14\\lib \
    -lopencv_ximgproc310 \
    -lopencv_xobjdetect310 \
    -lopencv_xphoto310 \
    -lopencv_aruco310 \
    -lopencv_bgsegm310 \
    -lopencv_bioinspired310 \
    -lopencv_calib3d310 \
    -lopencv_ccalib310 \
    -lopencv_core310 \
    -lopencv_datasets310 \
    -lopencv_dnn310 \
    -lopencv_dpm310 \
    -lopencv_face310 \
    -lopencv_features2d310 \
    -lopencv_flann310 \
    -lopencv_fuzzy310 \
    -lopencv_highgui310 \
    -lopencv_imgcodecs310 \
    -lopencv_imgproc310 \
    -lopencv_line_descriptor310 \
    -lopencv_ml310 \
    -lopencv_objdetect310 \
    -lopencv_optflow310 \
    -lopencv_photo310 \
    -lopencv_plot310 \
    -lopencv_reg310 \
    -lopencv_rgbd310 \
    -lopencv_saliency310 \
    -lopencv_shape310 \
    -lopencv_stereo310 \
    -lopencv_stitching310 \
    -lopencv_structured_light310 \
    -lopencv_superres310 \
    -lopencv_surface_matching310 \
    -lopencv_text310 \
    -lopencv_tracking310 \
    -lopencv_ts310 \
    -lopencv_video310 \
    -lopencv_videoio310 \
    -lopencv_videostab310 \
    -lopencv_xfeatures2d310
