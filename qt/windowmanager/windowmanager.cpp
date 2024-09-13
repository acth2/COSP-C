#include "windowmanager.h"
#include "win/topbar.h"
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
#include <QThread>
#include <QWindow>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#undef KeyPress

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
        
    windowCheckTimer = new QTimer(this);
    connect(windowCheckTimer, &QTimer::timeout, this, &WindowManager::checkForNewWindows);
    windowCheckTimer->start(500);
        
    showFullScreen();
}

Display *xDisplay;
void WindowManager::listExistingWindows() {
    Atom netWmWindowType = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE", False);
    Atom netWmWindowTypeDesktop = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_DESKTOP", False);

    Window windowRoot = DefaultRootWindow(xDisplay);
    Window parent, *children;
    unsigned int nChildren;

    if (XQueryTree(xDisplay, windowRoot, &windowRoot, &parent, &children, &nChildren)) {
        for (unsigned int i = 0; i < nChildren; i++) {
            Window child = children[i];

            Atom type;
            int format;
            unsigned long nItems, bytesAfter;
            unsigned char *data = nullptr;

            if (XGetWindowProperty(xDisplay, child, netWmWindowType, 0, 1, False, XA_ATOM,
                                   &type, &format, &nItems, &bytesAfter, &data) == Success) {
                if (data) {
                    Atom *atoms = (Atom *)data;
                    if (atoms[0] == netWmWindowTypeDesktop) {
                        XFree(data);
                        continue;
                    }
                    XFree(data);
                }
            }

            if (!trackedWindows.contains(child)) {
                createAndTrackWindow(child);
            }
        }
        XFree(children);
    }
}
void WindowManager::checkForNewWindows() {
    xDisplay = XOpenDisplay(nullptr);
    listExistingWindows();
    processX11Events(); 
    cleanUpClosedWindows();
}

void WindowManager::trackWindowEvents(Window xorgWindowId) {
    xDisplay = XOpenDisplay(nullptr);
    XSelectInput(xDisplay, xorgWindowId, StructureNotifyMask);
}

void WindowManager::processX11Events() {
    XEvent event;
    while (XPending(xDisplay)) {
        XNextEvent(xDisplay, &event);
        if (event.type == ConfigureNotify) {
            XConfigureEvent xce = event.xconfigure;
            if (trackedWindows.contains(xce.window)) {
                QWindow *window = trackedWindows.value(xce.window);
                appendLog(QString("Window resized/moved: (%1, %2), Size: (%3x%4)")
                    .arg(xce.x).arg(xce.y)
                    .arg(xce.width).arg(xce.height));

                updateTaskbarPosition(window);
            }
        }
    }
}

void WindowManager::toggleConsole() {
    isConsoleVisible = !isConsoleVisible;
    logLabel->setVisible(isConsoleVisible);
    appendLog("Welcome into the DEBUG window (AKA: Where my nightmare comes true), Press ESC to exit it");
}

void WindowManager::createAndTrackWindow(WId xorgWindowId) {
    QWindow *window = QWindow::fromWinId(xorgWindowId);
    if (window) {
        QString windowTitle = window->title();
        
        if (windowTitle == "CWMUSRI") {
            appendLog("Skipping window: CWMUSRI");
            return;
        }

        trackedWindows.insert(xorgWindowId, window);

        QRect geometry = window->geometry();
        appendLog(QString("Detected new window: %1").arg(xorgWindowId));
        appendLog(QString("Window position: (%1, %2)").arg(geometry.x()).arg(geometry.y()));
        appendLog(QString("Window size: (%1, %2)").arg(geometry.width()).arg(geometry.height()));

        if (!window->windowState().testFlag(Qt::WindowFullScreen)) {
            TopBar *topBar = new TopBar(window, this);
            topBar->updateTitle(windowTitle);
            windowTopBars.insert(xorgWindowId, topBar);
            topBar->updatePosition();
        }
    }
}

void WindowManager::updateTaskbarPosition(QWindow *window) {
    if (windowTopBars.contains(window->winId())) {
        TopBar *topBar = windowTopBars.value(window->winId());
        QRect windowGeometry = window->geometry();
        int topbarHeight = 30;

        int topBarYPosition = windowGeometry.y() - topbarHeight;

        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();

        if (topBarYPosition < screenGeometry.y()) {
            window->setGeometry(windowGeometry.x(), screenGeometry.y() + topbarHeight,
                                windowGeometry.width(), windowGeometry.height());
        }

        topBar->setGeometry(windowGeometry.x(), windowGeometry.y() - topbarHeight, 
                            windowGeometry.width(), topbarHeight);
        topBar->show();
    }
}

void WindowManager::appendLog(const QString &message) {
    if (!loggedMessages.contains(message)) {
        loggedMessages.insert(message);
        QString currentText = logLabel->text();
        logLabel->setText(currentText + "\n" + message);
    }
}

bool WindowManager::event(QEvent *qtEvent) {
    if (qtEvent->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(qtEvent);
        konamiCodeHandler->handleKeyPress(keyEvent);
    } else if (qtEvent->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(qtEvent);
        if (mouseEvent->button() == Qt::RightButton) {
            if (!userInteractRightWidget) {
                userInteractRightWidget = new UserInteractRight(this);
            }
            userInteractRightWidget->move(mouseEvent->globalPos());
            userInteractRightWidget->show();
        }
    }

    return QWidget::event(qtEvent);
}

void WindowManager::cleanUpClosedWindows() {
    QList<Window> windowsToRemove;
    for (auto xorgWindowId : trackedWindows.keys()) {
        XWindowAttributes attributes;
        int status = XGetWindowAttributes(xDisplay, xorgWindowId, &attributes);

        if (status == 0 || attributes.map_state == IsUnmapped) {
            windowsToRemove.append(xorgWindowId);
        }
    }

    for (auto xorgWindowId : windowsToRemove) {
        QWindow *window = trackedWindows.value(xorgWindowId);
        trackedWindows.remove(xorgWindowId);

        if (windowTopBars.contains(xorgWindowId)) {
            TopBar *topBar = windowTopBars.value(xorgWindowId);
            topBar->hide();
            topBar->deleteLater();
            windowTopBars.remove(xorgWindowId);
        }
    }
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
