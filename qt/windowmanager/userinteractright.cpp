#include "userinteractright.h"
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>

UserInteractRight::UserInteractRight(QWidget *parent)
    : QWidget(parent), isMouseInside(false) {

    setupUI();

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: rgba(255, 255, 255, 200); border: 2px solid black;");
}

void UserInteractRight::setupUI() {
    button1 = new QPushButton("Button 1", this);
    button2 = new QPushButton("Button 2", this);
    button3 = new QPushButton("Button 3", this);
    textLabel = new QLabel("This is a text label", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(textLabel);
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);

    layout->setContentsMargins(10, 10, 10, 10);
    setLayout(layout);
}

void UserInteractRight::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        QPoint pos = event->globalPos();
        move(pos.x() - 100, pos.y() - 50);
        show();
    }
}

void UserInteractRight::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        checkOutsideClick(event);
    }
}

void UserInteractRight::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(QBrush(QColor(255, 255, 255, 200)));
    painter.drawRect(rect());
}

void UserInteractRight::checkOutsideClick(QMouseEvent *event) {
    QPoint globalPos = event->globalPos();
    QRect rect = geometry();

    if (!rect.contains(globalPos)) {
        close();
    }
}
