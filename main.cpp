#include <QApplication>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <QPushButton>
#include <iostream>
#include <QLineEdit>
#include "QWidget"
#include <QMainWindow>
#include <QDesktopWidget>
#include "window.h"




int main (int argc, char **argv)
{
    QApplication app (argc, argv);
    Window window;
    window.show();


    QObject::connect(window.confirmSettings,SIGNAL(clicked()),&window,SLOT(alibava_reader()));



    return app.exec();
}
