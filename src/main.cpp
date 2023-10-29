#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyleSheet("QMessageBox{  border: 2px solid red; border-radius: 6px; padding: 0 8px; background-color: rgb(211, 215, 207);} QMessageBox QLabel {color: rgb(32, 74, 135); font: 15px; };");
    MainWindow w;
    w.show();
    return a.exec();
}
