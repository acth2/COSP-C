#include "windowmanager.h"
#include "userinteractright.h"
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
    logLabel->setVisible(false);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(logLabel);
    layout->setContentsMargins(10, 10, 10, 10);
    setLayout(layout);

    konamiCodeHandler = new KonamiCodeHandler(this);
    connect(konamiCodeHandler, &KonamiCodeHandler::konamiCodeEntered, this, &WindowManager::toggleConsole);

    interactWidget = new UserInteractRight(this);

    showFullScreen();
}

void WindowManager::toggleConsole() {
    isConsoleVisible = !isConsoleVisible;
    logLabel->setVisible(isConsoleVisible);
    appendLog("Welcome into the DEBUG window (AKA: Where my nightmare comes true), Press ESC to exit it");
}

void WindowManager::appendLog(const QString &message) {
    if (!loggedMessages.contains(message)) {
        loggedMessages.insert(message);
        QString currentText = logLabel->text();
        logLabel->setText(currentText + "\n" + message);
    }
}

bool WindowManager::event(QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        konamiCodeHandler->handleKeyPress(keyEvent);
    } else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            interactWidget->mousePressEvent(mouseEvent);
        }
    }
    return QWidget::event(event);
}

void WindowManager::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape && logLabel->isVisible()) {
        toggleConsole();
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
        appendLog("Background loaded!");
        painter.drawPixmap(0, 0, width(), height(), backgroundPixmap);
    }
}
