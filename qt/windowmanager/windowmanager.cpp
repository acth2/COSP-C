#include "windowmanager.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QDebug>

WindowManager::WindowManager(QWidget *parent) : QWidget(parent) {
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry);
    } else {
        qDebug() << "Erreur : Impossible d'obtenir les informations sur l'écran.";
    }

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

bool WindowManager::event(QEvent *event) {
    if (event->type() == QEvent::WindowActivate) {
        qDebug() << "Window activated";
    }
    return QWidget::event(event);
}
