#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->playBtn,&QPushButton::released,
            this,[=](){
        ui->ffwidget->open("../../video/supermarket.mp4");
    });

    connect(ui->ffwidget,SIGNAL(durationRead(long long)),
            this,SLOT(onDurationRead(long long)));

    connect(ui->ffwidget,SIGNAL(playProccessed(long long)),
            this,SLOT(onPlayProcessed(long long)));

    connect(ui->playHSlider,&QSlider::sliderPressed,
            this,[=](){
        disconnect(ui->ffwidget,SIGNAL(playProccessed(long long)),
                   this,SLOT(onPlayProcessed(long long)));
    });

    connect(ui->playHSlider,&QSlider::sliderReleased,
            this,[=](){
        int smax;
        smax=ui->playHSlider->maximum()+1;
        double ratio;
        ratio=ui->playHSlider->value()*1.0/smax;
        ui->ffwidget->seek(ratio);
        qDebug() << "debug10" << ratio;
        connect(ui->ffwidget,SIGNAL(playProccessed(long long)),
                this,SLOT(onPlayProcessed(long long)));
    });
}

void MainWindow::onDurationRead(long long dur)
{
    ui->playHSlider->setMaximum(dur);
}

void MainWindow::onPlayProcessed(long long ms)
{
    ui->playHSlider->setValue(ms);
}

MainWindow::~MainWindow()
{
    delete ui;
}
