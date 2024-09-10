#include "windowmanager.h"
#include "Window.h"
#include "userinteractright.h"
#include "taskbar.h"
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QKeyEvent>
#include <QSet>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QProcess>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

WindowManager::WindowManager(QWidget *parent)
    : QWidget(parent), 
      isConsoleVisible(false),
      userInteractRightWidget(nullptr),
      backgroundImagePath("/usr/cydra/backgrounds/current.png") {
    
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry);
    }

    display = XOpenDisplay(nullptr);
    if (!display) {
        appendLog("Cannot open X11 display");
        return;
    }
    root = DefaultRootWindow(display);

    XSelectInput(display, root, SubstructureNotifyMask);

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
        
    userInteractRightWidget = nullptr;

    showFullScreen();
}

void WindowManager::monitorXorgWindows() {
    XEvent event;
    while (true) {
        XNextEvent(display, &event);

        if (event.type == MapNotify) {
            Window xorgWindow = event.xmap.window;
            appendLog("New Xorg window detected");

            attachTaskbarToWindow(xorgWindow);
        }
    }
}


void WindowManager::toggleConsole() {
    isConsoleVisible = !isConsoleVisible;
    logLabel->setVisible(isConsoleVisible);
    appendLog("Welcome into the DEBUG window (AKA: Where my nightmare comes true), Press ESC to exit it");
}
void WindowManager::attachTaskbarToWindow(WId xorgWindowId) {
    Main::Window *appWindow = new Main::Window();

    appWindow->setXorgAppWindow(xorgWindowId);
    appWindow->show();
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
            if (!userInteractRightWidget) {
                userInteractRightWidget = new UserInteractRight(this);
            }
            userInteractRightWidget->move(mouseEvent->globalPos());
            userInteractRightWidget->show();
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
