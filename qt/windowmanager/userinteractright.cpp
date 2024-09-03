#include "userinteractright.h"

UserInteractRight::UserInteractRight(QWidget *parent)
    : QWidget(parent) {
    setFixedSize(200, 150);

    layout = new QVBoxLayout(this);
    
    textLabel = new QLabel("Some Text", this);
    button1 = new QPushButton("Button 1", this);
    button2 = new QPushButton("Button 2", this);
    button3 = new QPushButton("Button 3", this);

    layout->addWidget(textLabel);
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->setContentsMargins(10, 10, 10, 10);
    setLayout(layout);
}

void UserInteractRight::mousePressEvent(QMouseEvent *event) {
    event->ignore();
}
