#include "mainwindow.h"
#include <QApplication>
//#pragma comment(lib,"avcodec.lib")
//#pragma comment(lib,"avformat.lib")
//#pragma comment(lib,"avutil.lib")
//#pragma comment(lib,"swscale.lib")
//#pragma comment(lib,"swresample.lib ")
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
