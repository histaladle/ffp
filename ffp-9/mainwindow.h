#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ffwidget.h"
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
    void onDurationRead(long long dur);
    void onPlayProcessed(long long ms);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
