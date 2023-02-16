#include "graphwidget.h"
#include "mymainwindow.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <limits>
#include <set>

#include <QLayout>
#include <QPainter>
#include <QWidget>
#include <QPushButton>

#define MAX_VALUES 100

graphWidget::graphWidget(int fd, QWidget *parent)
    : QWidget(parent)
    , tempIndex_(0)
    , fd_(fd) 
{
}

void graphWidget::paintEvent(QPaintEvent*)
{
    QPainter *pinceau = new QPainter(this);

    // repère en bas à gauche
    pinceau->translate(0, height());
    pinceau->scale(1.0, -1.0);

    const int MULTIPLICATOR = 100;

    pinceau->setBrush(Qt::green);

    float minTemp = std::numeric_limits<float>().max();
    float maxTemp = std::numeric_limits<float>().min();
    for (size_t i = 0; i < temperatures_.size(); ++i)
    {
        float newTemp = temperatures_[i];
        if (newTemp < minTemp)
        {
            minTemp = newTemp;
        }
        else if (newTemp > maxTemp)
        {
            maxTemp = newTemp;
        }
    }

    float minHeight = 10.f, maxHeight = (size().height() - 20.f);
    float widthPerBar = (size().width() - 20.f) / 100.f;


    // data
    int xPos = 15;

    for (size_t i = 0; i < temperatures_.size(); i++)
    {
        float barHeight = minHeight + ((temperatures_[i] - minTemp) / (maxTemp - minTemp)) * (maxHeight - minHeight);

        // Draw bar
        pinceau->drawRect(xPos, 15, widthPerBar, barHeight); // (x,y,w,h)

        // add value on bar
        pinceau->resetTransform();
        QString value = QString::number(temperatures_[i], 'f', 2);
        pinceau->drawText(xPos + 3, height() - 20 - barHeight, value);

        // put the coordonate system back in place
        pinceau->resetTransform();
        pinceau->translate(0, height());
        pinceau->scale(1.0, -1.0);

        xPos += widthPerBar;
    }

    // axes, figures
    pinceau->setBrush(Qt::black);
    // axe x
    pinceau->drawRect(10,10, width(), 2);
    // axe y
    pinceau->drawRect(10,10, 2, height());
    pinceau->resetTransform();
    pinceau->drawText(width()-50, height()-15, "Temps");
    pinceau->rotate(-90);
    pinceau->drawText(-80, 25, "Température");

    pinceau->end();
}

void graphWidget::readTemperature()
{
    int ret = read(fd_, nullptr, 0);
    float temperature = 175.f * ret / 65536.f - 45.f;


    if (temperatures_.size() < MAX_VALUES)
    {
        temperatures_.push_back(temperature);
    }
    else
    {
        temperatures_[tempIndex_] = temperature;
    }
    if (++tempIndex_ >= MAX_VALUES)
    {
        for (size_t i = 1; i < temperatures_.size(); ++i)
        {
            temperatures_[i - 1] = temperatures_[i];
        }
        tempIndex_ = 0;
    }

    repaint();
}