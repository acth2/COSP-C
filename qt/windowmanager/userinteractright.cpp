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

    // Modern UI Styling
    QString buttonStyle = R"(
        QPushButton {
            background-color: #0078D4; /* Windows 11 Blue */
            color: white;
            border: none;
            border-radius: 5px; /* Slightly rounded corners */
            padding: 8px 16px;
            font-size: 12px;
            font-weight: bold;
            min-width: 100px; /* Ensure buttons have a minimum width */
        }
        QPushButton:hover {
            background-color: #005A9E; /* Darker blue on hover */
        }
    )";

    QString labelStyle = R"(
        QLabel {
            color: #333333; /* Dark text color */
            font-size: 14px;
            font-weight: medium;
            margin-bottom: 10px;
            background-color: transparent;
        }
    )";

    textLabel->setStyleSheet(labelStyle);
    textLabel->setAlignment(Qt::AlignCenter);
    button1->setStyleSheet(buttonStyle);
    button2->setStyleSheet(buttonStyle);
    button3->setStyleSheet(buttonStyle);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(textLabel);
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->setSpacing(10);
    layout->setContentsMargins(15, 15, 15, 15);
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

    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(QColor(255, 255, 255, 240));
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
