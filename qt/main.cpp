/**
 * @file   qt_drvSHTC.c
 * @author Alexandre Besia, Filippos Gleglakos
 * @date   30.05.2022
 * @version 0.1
 * @brief  A Linux user space program that communicates with the drvSHTC driver.
 * It reads the temperature from the driver.
 * For this example to work the device must be called /dev/drvSHTC.
*/

#include <QApplication>

#include "mymainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyMainWindow w;
    w.show();
    return app.exec();
}