#include "userinteractright.h"
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QApplication>

UserInteractRight::UserInteractRight(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setupUI();
    setFixedSize(200, 250);
}

void UserInteractRight::setupUI() {
    button1 = new QPushButton("Exit system", this);
    button2 = new QPushButton("Button 2", this);
    button3 = new QPushButton("Button 3", this);
    textLabel = new QLabel("This is a text label", this);

    button1->setStyleSheet("QPushButton { background-color: lightgray; border: none; padding: 10px; }"
                           "QPushButton:hover { background-color: gray; }");
    button2->setStyleSheet("QPushButton { background-color: lightgray; border: none; padding: 10px; }"
                           "QPushButton:hover { background-color: gray; }");
    button3->setStyleSheet("QPushButton { background-color: lightgray; border: none; padding: 10px; }"
                           "QPushButton:hover { background-color: gray; }");

    textLabel->setStyleSheet("QLabel { background-color: transparent; padding: 10px; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(textLabel);
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->setContentsMargins(10, 10, 10, 10);
    setLayout(layout);

    connect(button1, &QPushButton::clicked, this, &UserInteractRight::button1Clicked);
}

void UserInteractRight::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        isMousePressed = true;
        QPoint cursorPos = event->globalPos();
        move(cursorPos.x() - width() / 2, cursorPos.y() - height() / 2);
        show();
    }
    QWidget::mousePressEvent(event);
}

void UserInteractRight::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        isMousePressed = false;
        closeIfClickedOutside(event);
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

void UserInteractRight::button1Clicked() {
    QApplication::quit();
}

void UserInteractRight::closeIfClickedOutside(QMouseEvent *event) {
    if (!rect().contains(mapFromGlobal(event->globalPos()))) {
        close();
    }
}
