#include "userinteractright.h"
#include "windowmanager.h"
#include <QPainter>
#include <QApplication>

UserInteractRight::UserInteractRight(WindowManager *windowManager, QWidget *parent)
    : QWidget(parent), windowManager(windowManager), isMouseInside(false) {

    setupUI();

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: rgba(255, 255, 255, 200); border: 2px solid black;");
}

void UserInteractRight::setupUI() {
    button1 = new QPushButton("Exit the system", this);
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

    connect(button1, &QPushButton::clicked, this, &UserInteractRight::button1Clicked);
    connect(button2, &QPushButton::clicked, this, &UserInteractRight::button2Clicked);
    connect(button3, &QPushButton::clicked, this, &UserInteractRight::button3Clicked);
}

void UserInteractRight::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        QPoint pos = event->globalPos();
        move(pos.x() - 7, pos.y() + 10);
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

void UserInteractRight::button1Clicked() {
    QApplication::quit();
}
