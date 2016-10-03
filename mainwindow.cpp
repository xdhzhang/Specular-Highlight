#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QTimer>
#include <QFileDialog>

#include "Basic/basic.h"

#define QCV_DEF_ITERATION_VALUE                     0
#define QCV_DEF_QTIMER_VALUE                        20

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow) {
    m_ui->setupUi(this);

    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(cam()));
}

MainWindow::~MainWindow() {
    delete m_ui;
}

void MainWindow::cam() {
    static QMat frame;
    m_webCam >> frame;

    if(frame.empty()) {
        qDebug() << "Frame empty";
        return;
    }

    frame.cvtColor_(CV_BGR2RGB);

    highLightRemove(frame, QCV_DEF_ITERATION_VALUE);
}

void MainWindow::on_pbCamOnOff_clicked() {
    if(m_ui->pbCamOnOff->text() == "Start") {
        m_webCam.open(m_ui->sbCamNumber->value());
        if(!m_webCam.isOpened()) {
            qDebug() << "Not found web cam";
            qDebug() << "Automatic setting";
            m_webCam.open(0);
            if(!m_webCam.isOpened()) {
                qDebug() << "Not found web cam";
                return;
            }
        }

        m_ui->pbCamOnOff->setText("Stop");
        m_ui->pbFileOpen->setEnabled(false);
        m_timer->start(QCV_DEF_QTIMER_VALUE);
    } else {
        m_ui->pbCamOnOff->setText("Start");
        m_ui->pbFileOpen->setEnabled(true);
        m_timer->stop();
    }
}

void MainWindow::on_pbFileOpen_clicked() {
    QString pathFile = QFileDialog::getOpenFileName();

    if(pathFile.isEmpty()) {
        qDebug() << "Path file empty";
        return;
    }

    QMat file = imread(pathFile.toStdString());

    if(file.empty()) {
        qDebug() << "File empty";
        return;
    }

    file.cvtColor_(CV_BGR2RGB);

    highLightRemove(file, QCV_DEF_ITERATION_VALUE);
}

void MainWindow::highLightRemove(QMat mat, int iter) {
    static QHighLight h;
    h.init(mat, 20, m_ui->doubleSpinBox->value());
    int iter_count = h.diffuse(iter);

//    qDebug() << "Highlight removal of iterations before convergece: " << iter_count;

    h.getDiffuseImage().draw(m_ui->lblImage);
}

