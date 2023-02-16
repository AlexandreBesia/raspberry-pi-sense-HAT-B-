#pragma once

#include <QWidget>
#include <QPushButton>

#include <vector>

class graphWidget : public QWidget
{
    Q_OBJECT

public:
    graphWidget(int fd, QWidget *parent = nullptr);

    void paintEvent(QPaintEvent*);
    void readTemperature();

private:
    std::vector<float> temperatures_;
    size_t tempIndex_;
    int fd_;
};