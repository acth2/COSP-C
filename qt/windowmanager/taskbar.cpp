#include "taskbar.h"
#include <QPropertyAnimation>
#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>

TaskBar::TaskBar(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setStyleSheet("background-color: #333333;");

    startButton = new QPushButton(this);
    startButton->setIcon(QIcon("/usr/cydra/cydra.png"));
    startButton->setIconSize(QSize(32, 32));
    startButton->setStyleSheet("border: none;");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(startButton, 0, Qt::AlignLeft | Qt::AlignBottom);
    layout->setContentsMargins(5, 5, 5, 5);
    setLayout(layout);

    popup = new QLabel(this);
    popup->setGeometry(0, -200, 200, 200);
    popup->setStyleSheet("background-color: white; border: 1px solid gray;");
    popup->hide();

    connect(startButton, &QPushButton::clicked, this, &TaskBar::showPopup);

    adjustSizeToScreen();
}

void TaskBar::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    adjustSizeToScreen();
}

void TaskBar::adjustSizeToScreen() {
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        setFixedSize(screenGeometry.width(), 40);
        move(0, screenGeometry.height() - height());
    }
}

void TaskBar::showPopup() {
    popup->setGeometry(startButton->x(), startButton->y() - popup->height(), popup->width(), popup->height());
    popup->show();
    
    QPropertyAnimation *animation = new QPropertyAnimation(popup, "geometry");
    animation->setDuration(500);
    animation->setStartValue(QRect(popup->x(), popup->y() + popup->height(), popup->width(), popup->height()));
    animation->setEndValue(QRect(popup->x(), popup->y(), popup->width(), popup->height()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void TaskBar::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        closePopup();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void TaskBar::mousePressEvent(QMouseEvent *event) {
    if (popup->isVisible() && !popup->rect().contains(event->pos()) && !startButton->rect().contains(event->pos())) {
        closePopup();
    } else {
        QWidget::mousePressEvent(event);
    }
}

void TaskBar::closePopup() {
    popup->hide();
}
