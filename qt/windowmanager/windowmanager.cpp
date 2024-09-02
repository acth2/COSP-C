#include "windowmanager.h"
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QKeyEvent>
#include <QProcess>
#include <QCloseEvent>

WindowManager::WindowManager(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry);
        qDebug() << "Screen size:" << screenGeometry.size();
    } else {
        qDebug() << "Erreur : Impossible d'obtenir les informations sur l'écran.";
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
    qDebug() << "Touche pressée : " << event->key();
    if (event->key() == Qt::Key_P) {
        qDebug() << "Touche 'p' pressée : Ouverture de TWM";
        QProcess::execute("startx /usr/bin/twm");
    } else {
        QWidget::keyPressEvent(event);
    }
}

void WindowManager::closeEvent(QCloseEvent *event) {
    event->ignore();
}
