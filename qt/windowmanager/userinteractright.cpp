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

void UserInteractRight::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        isMousePressed = true;
        QPoint pos = event->globalPos();
        move(pos.x() - 5, pos.y() + 10); 
        show();
    }
    QWidget::mousePressEvent(event);
}

void UserInteractRight::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        isMousePressed = false;
        QPoint pos = event->globalPos();
        if (!rect().contains(mapFromGlobal(pos))) {
            close();
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void UserInteractRight::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(Qt::gray);
    painter.setBrush(Qt::lightGray);
    painter.drawRect(rect());

    QWidget::paintEvent(event);
}

void UserInteractRight::checkOutsideClick(const QPoint &pos) {
    if (!rect().contains(mapFromGlobal(pos))) {
        close();
    }
}

void UserInteractRight::button1Clicked() {
    QApplication::quit();
}
