#pragma once

#include <QMainWindow>

#include "graphwidget.h"

class MyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MyMainWindow(QWidget *parent = nullptr);
    
    virtual void timerEvent(QTimerEvent*) override;

private:
    QAction *actQuit, *actConnet;
    QMenu *menu;
    graphWidget *widgetCentral;

public slots:
    void quitter();
};