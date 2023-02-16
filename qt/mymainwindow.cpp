#include "mymainwindow.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <QAction>
#include <QMenuBar>
#include <QApplication>

MyMainWindow::MyMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    int fd = open("/dev/drvSHTC", O_RDWR);
    if (fd < 0)
    {
        return;
    }
    
    setFixedSize(1280, 720);
    
    // widget central
    widgetCentral = new graphWidget(fd, this);
    setCentralWidget(widgetCentral);

    // actions
    actQuit = new QAction("quitter", this);
    actQuit->setShortcut(tr("Ctrl+Q"));
    connect(actQuit, &QAction::triggered, this, &MyMainWindow::quitter);
        
    startTimer(200.f);
}
    
void MyMainWindow::timerEvent(QTimerEvent*)
{
    widgetCentral->readTemperature();
}


void MyMainWindow::quitter()
{
    qApp->quit();
}