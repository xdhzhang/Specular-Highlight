#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/highgui/highgui.hpp>

#include "QHighLight/qhighlight.h"

using namespace qcv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void cam();

    void on_pbCamOnOff_clicked();

    void on_pbFileOpen_clicked();

private:
    void highLightRemove(QMat mat, int iter = 0);

    Ui::MainWindow *m_ui;
    VideoCapture m_webCam;
    QTimer *m_timer;
};

#endif // MAINWINDOW_H
