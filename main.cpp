#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();

    a.setStyleSheet("QGroupBox {font-weight: bold; border: 1px solid gray; border-radius: 2px; margin-top: .5em; padding-top: .2em} QGroupBox::title {subcontrol-origin: margin; left: 6px; padding: 0 3px;}");

    return a.exec();
}
