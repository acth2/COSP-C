#include "windowmanager.h"
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QKeyEvent>
#include <QProcess>
#include <QCloseEvent>
#include <QPainter>
#include <QPixmap>

WindowManager::WindowManager(QWidget *parent)
    : QWidget(parent), backgroundImagePath("/usr/cydra/backgrounds/current.png") {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry);
    }
    showFullScreen();
}

bool WindowManager::event(QEvent *event) {
    if (event->type() == QEvent::WindowActivate) {
        qDebug() << "Window activated";
    }
    return QWidget::event(event);
}

void WindowManager::keyPressEvent(QKeyEvent *event) {
    qDebug() << "Key pressed:" << event->key();
    if (event->key() == Qt::Key_P) {
        qDebug() << "Key 'P' pressed: Closing X server";
        QProcess::execute("pkill Xorg");
    } else {
        QWidget::keyPressEvent(event);
    }
}

void WindowManager::closeEvent(QCloseEvent *event) {
    qDebug() << "Close attempt ignored";
    event->ignore();
}

void WindowManager::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPixmap backgroundPixmap(backgroundImagePath);

    if (!backgroundPixmap.isNull()) {
        qDebug() << "Background image loaded successfully.";
        QPixmap scaledPixmap = backgroundPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, scaledPixmap);
    } else {
        qDebug() << "Failed to load background image. Filling with white.";
        painter.fillRect(rect(), Qt::white);
    }
}
