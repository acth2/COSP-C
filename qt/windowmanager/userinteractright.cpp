#include "userinteractright.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QApplication>

UserInteractRight::UserInteractRight(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    button1 = new QPushButton("Exit system", this);
    button2 = new QPushButton("Button 2", this);
    button3 = new QPushButton("Button 3", this);
    textLabel = new QLabel("This is a text label", this);

    button1->setStyleSheet("QPushButton { background-color: lightgray; border: none; }");
    button2->setStyleSheet("QPushButton { background-color: lightgray; border: none; }");
    button3->setStyleSheet("QPushButton { background-color: lightgray; border: none; }");

    textLabel->setStyleSheet("QLabel { background-color: transparent; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(textLabel);
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->setContentsMargins(10, 10, 10, 10);
    setLayout(layout);

    connect(button1, &QPushButton::clicked, this, &UserInteractRight::button1Clicked);

    setFixedSize(200, 200);
}
