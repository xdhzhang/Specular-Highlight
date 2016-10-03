#include "QMat/qmat.h"

#include <QDebug>

#include <opencv2/imgproc/imgproc.hpp>

using namespace qcv;

QMat& QMat::operator = (const MatExpr& expr) {
    *this = QMat(expr);
    return *this;
}

QMat& QMat::operator = (const Scalar& s) {
    *this = QMat(s);
    return *this;
}

void QMat::draw(QLabel *lable) {
    switch(this->type()) {

    case CV_8UC4: {
        QImage image(this->data,
                     this->cols,
                     this->rows,
                     this->step,
                     QImage::Format_RGB32);
//        qDebug() << "CV_8UC4";
        lable->setPixmap(QPixmap::fromImage(image));
        break;
    }
    case CV_8UC3: {
        QImage image(this->data,
                     this->cols,
                     this->rows,
                     this->step,
                     QImage::Format_RGB888);
//        qDebug() << "CV_8UC3";
        lable->setPixmap(QPixmap::fromImage(image));
        break;
    }
    case CV_8UC1: {
        static QVector<QRgb> colorTable;
        if(colorTable.isEmpty())
            for(int i = 0; i < 256; ++i)
                colorTable.push_back(qRgb(i, i, i));
        QImage image(this->data,
                     this->cols,
                     this->rows,
                     this->step,
                     QImage::Format_Indexed8);
        image.setColorTable(colorTable);
//        qDebug() << "CV_8UC1";
        lable->setPixmap(QPixmap::fromImage(image));
        break;
    }
    case CV_32FC1: {
        QImage image(this->data,
                     this->cols,
                     this->rows,
                     this->step,
                     QImage::Format_ARGB32);
//        qDebug() << "CV_32FC1";
        lable->setPixmap(QPixmap::fromImage(image));
        break;
    }

    default: {
        qDebug() << "There is no corresponding type" << this->type();
        lable->setPixmap(QPixmap::fromImage(QImage()));
        break;
    }

    }
}

QMat QMat::cvtColor(int code, int dstCn) {
    QMat dst;
    cv::cvtColor(*this, dst, code, dstCn);
    return dst;
}

void QMat::cvtColor_(int code, int dstCn) {
    cv::cvtColor(*this, *this, code, dstCn);
}
