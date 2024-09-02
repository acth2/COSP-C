#include "windowmanager.h"
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QKeyEvent>
#include <QProcess>

WindowManager::WindowManager(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

bool WindowManager::event(QEvent *event) {
    if (event->type() == QEvent::WindowActivate) {
        qDebug() << "Window activated";
    }
    return QWidget::event(event);
}

void WindowManager::keyPressEvent(QKeyEvent *event) {
    qDebug() << "Touche pressée : " << event->key();
    if (event->key() == Qt::Key_P) {
        qDebug() << "Touche 'p' pressée : fermeture du serveur X";
        QProcess::execute("pkill Xorg");
    } else {
        QWidget::keyPressEvent(event);
    }
}
