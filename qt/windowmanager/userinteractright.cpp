#include "userinteractright.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QApplication>

UserInteractRight::UserInteractRight(QWidget *parent)
    : QWidget(parent) {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    exitButton = new QPushButton("Exit system", this);
    textLabel = new QLabel("Ceci est un test", this);

    exitButton->setStyleSheet("QPushButton { background-color: lightgray; border: none; }");
    textLabel->setStyleSheet("QLabel { background-color: transparent; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(textLabel);
    layout->addWidget(exitButton);
    layout->setContentsMargins(10, 10, 10, 10);
    setLayout(layout);

    connect(exitButton, &QPushButton::clicked, QApplication::instance(), &QApplication::quit);

    setFixedSize(200, 100);
}

void UserInteractRight::mousePressEvent(QMouseEvent *event) {
    close();
    if (event->button() == Qt::RightButton) {
        QPoint pos = event->globalPos();
        move(pos.x() - width() / 2, pos.y() - height() / 2); 
        show();
    } else {
        closeIfClickedOutside(event->pos());
    }
}

void UserInteractRight::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    }
    QWidget::keyPressEvent(event);
}

void UserInteractRight::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(Qt::gray);
    painter.setBrush(Qt::lightGray);
    painter.drawRect(rect());

    QWidget::paintEvent(event);
}

void UserInteractRight::closeIfClickedOutside(const QPoint &pos) {
    if (!rect().contains(pos)) {
        close();
    }
}
