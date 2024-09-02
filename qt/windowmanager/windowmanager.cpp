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
#include <QLabel>
#include <QVBoxLayout>

WindowManager::WindowManager(QWidget *parent)
    : QWidget(parent), backgroundImagePath("/usr/cydra/backgrounds/current.png") {
    
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry);
    }

    logLabel = new QLabel(this);
    logLabel->setStyleSheet("QLabel { color : white; background-color : rgba(0, 0, 0, 150); }");
    logLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(logLabel);
    layout->setContentsMargins(10, 10, 10, 10);
    setLayout(layout);

    showFullScreen();
}

void WindowManager::appendLog(const QString &message) {
    QString currentText = logLabel->text();
    logLabel->setText(currentText + "\n" + message);
}

bool WindowManager::event(QEvent *event) {
    if (event->type() == QEvent::WindowActivate) {
        appendLog("Window activated");
    }
    return QWidget::event(event);
}

void WindowManager::keyPressEvent(QKeyEvent *event) {
    appendLog("Key pressed: " + QString::number(event->key()));
    if (event->key() == Qt::Key_P) {
        appendLog("Key 'P' pressed: closing X server");
        QProcess::execute("pkill Xorg");
    } else {
        QWidget::keyPressEvent(event);
    }
}

void WindowManager::closeEvent(QCloseEvent *event) {
    appendLog("Close attempt ignored");
    event->ignore();
}

void WindowManager::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPixmap backgroundPixmap(backgroundImagePath);
    if (!backgroundPixmap.isNull()) {
        appendLog("Background image loaded successfully.");
        QPixmap scaledPixmap = backgroundPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, scaledPixmap);
    } else {
        appendLog("Failed to load background image. Filling with white.");
        painter.fillRect(rect(), Qt::white);
    }
}
