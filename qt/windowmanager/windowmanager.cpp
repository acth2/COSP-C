#include "windowmanager.h"
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QKeyEvent>
#include <QProcess>
#include <QCloseEvent>
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QTextStream>
#include <iostream>

WindowManager::WindowManager(QWidget *parent) : QWidget(parent), backgroundImagePath("/usr/cydra/backgrounds/current.png") {
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
        logDebug("Window activated");
    }
    return QWidget::event(event);
}

void WindowManager::keyPressEvent(QKeyEvent *event) {
    logDebug("Touche pressée : " + QString::number(event->key()));
    if (event->key() == Qt::Key_P) {
        logDebug("Touche 'p' pressée : fermeture du serveur X");
        QProcess::execute("pkill Xorg");
    } else {
        QWidget::keyPressEvent(event);
    }
}

void WindowManager::closeEvent(QCloseEvent *event) {
    logDebug("Tentative de fermeture ignorée");
    event->ignore();
}

void WindowManager::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPixmap backgroundPixmap(backgroundImagePath);
    if (!backgroundPixmap.isNull()) {
        logDebug("Image de fond chargée avec succès.");
        painter.drawPixmap(0, 0, width(), height(), backgroundPixmap);
    }
}
