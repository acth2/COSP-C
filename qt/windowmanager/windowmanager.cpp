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
#include <QResizeEvent>
#include <QDateTime>
#include <QLabel>
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
    windowCheckTimer->start(50);
        
    showFullScreen();
}

Display *xDisplay;
void WindowManager::listExistingWindows() {
    if (xDisplay) {
        Atom netWmWindowType = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE", False);
        Atom netWmWindowTypeNormal = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_NORMAL", False);
        Atom netWmWindowTypeDesktop = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
        Atom netWmWindowTypeDock = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_DOCK", False);
        Atom netWmWindowTypeToolbar = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_TOOLBAR", False);
        Atom netWmWindowTypeMenu = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_MENU", False);
        Atom netWmWindowTypeUtility = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_UTILITY", False);
        Atom netWmWindowTypeSplash = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_SPLASH", False);
        Atom netWmWindowTypeDialog = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_DIALOG", False);
        
        Window windowRoot = DefaultRootWindow(xDisplay);
        Window parent, *children;
        unsigned int nChildren;

        if (XQueryTree(xDisplay, windowRoot, &windowRoot, &parent, &children, &nChildren)) {
            for (unsigned int i = 0; i < nChildren; i++) {
                Window child = children[i];

                char *windowName = nullptr;
                if (XFetchName(xDisplay, child, &windowName) && windowName) {
                    QString name(windowName);
                    if (name == "QTerminal") {
                        appendLog("INFO: Detected QTerminal window: " + QString::number(child));
                        createAndTrackWindow(child);
                        XFree(windowName);
                        continue;
                    }
                    XFree(windowName);
                }

                Atom type;
                int format;
                unsigned long nItems, bytesAfter;
                unsigned char *data = nullptr;

                if (XGetWindowProperty(xDisplay, child, netWmWindowType, 0, 1, False, XA_ATOM,
                                   &type, &format, &nItems, &bytesAfter, &data) == Success) {
                    if (data) {
                        Atom *atoms = (Atom *)data;
                        if (atoms[0] != netWmWindowTypeNormal &&
                            atoms[0] != netWmWindowTypeDesktop &&
                            atoms[0] != netWmWindowTypeDock &&
                            atoms[0] != netWmWindowTypeToolbar &&
                            atoms[0] != netWmWindowTypeMenu &&
                            atoms[0] != netWmWindowTypeUtility &&
                            atoms[0] != netWmWindowTypeSplash &&
                            atoms[0] != netWmWindowTypeDialog) {
                            appendLog("INFO: Skipping non-desktop-dock-toolbar-menu-utility-splash-dialog window: " + QString::number(child));
                            XFree(data);
                            continue;
                        }
                    }
                }

                XWindowAttributes attributes;
                if (XGetWindowAttributes(xDisplay, child, &attributes) == 0 || attributes.map_state != IsViewable) {
                    appendLog("INFO: Skipping attribute window: " + QString::number(child));
                    continue;
                }

                QRect windowGeometry(attributes.x, attributes.y, attributes.width, attributes.height);

                if (windowGeometry.width() == 0 || windowGeometry.height() == 0) {
                    appendLog("INFO: Skipping non-graphical window (0x0 size): " + QString::number(child));
                    continue;
                }

                appendLog("INFO: Detected graphical X11 window: " + QString::number(child));
            
                if (!trackedWindows.contains(child)) {
                    createAndTrackWindow(child);
                }
            }
            XFree(children);
        }
    } else {
        appendLog("ERR: Failed to open X Display ..");
    }
}

void WindowManager::checkForNewWindows() {
    xDisplay = XOpenDisplay(nullptr);
    if (xDisplay) {
        listExistingWindows();
        processX11Events(); 
        cleanUpClosedWindows();
        
        Window activeWindow;
        int revert;
        XGetInputFocus(xDisplay, &activeWindow, &revert);

        if (!trackedWindows.contains(activeWindow)) {
            appendLog("INFO: Focusing back to Qt window");
            this->activateWindow();
        }
        XCloseDisplay(xDisplay);
    } else {
        appendLog("ERR: Failed to open X Display ..");
    }
}

void WindowManager::trackWindowEvents(Window xorgWindowId) {
    xDisplay = XOpenDisplay(nullptr);
    if (xDisplay) {
        XSelectInput(xDisplay, xorgWindowId, StructureNotifyMask);
        
        XResizeWindow(xDisplay, xorgWindowId, 500, 500);
    } else {
        appendLog("ERR: Failed to open X Display ..");
    }
}

void WindowManager::processX11Events() {
    XEvent event;
    if (xDisplay) {
        while (XPending(xDisplay)) {
            XNextEvent(xDisplay, &event);
            if (event.type == ConfigureNotify) {
                XConfigureEvent xce = event.xconfigure;

                if (trackedWindows.contains(xce.window)) {
                    QWindow *window = trackedWindows.value(xce.window);
                    QRect windowGeometry = window->geometry();

                    appendLog(QString("INFO: Window resized/moved: (%1, %2), Size: (%3x%4)")
                        .arg(xce.x).arg(xce.y)
                        .arg(xce.width).arg(xce.height));

                    updateTaskbarPosition(window);
                }
            }
        }
    } else {
        appendLog("ERR: Failed to open X Display ..");
    }
}


void WindowManager::toggleConsole() {
    isConsoleVisible = !isConsoleVisible;
    logLabel->setVisible(isConsoleVisible);
    appendLog("Welcome into the DEBUG window (Where my nightmare comes true), Press ESC to exit it");
}
void WindowManager::createAndTrackWindow(WId xorgWindowId) {
    QWindow *x11Window = QWindow::fromWinId(xorgWindowId);
    if (x11Window) {
        trackedWindows.insert(xorgWindowId, x11Window);
        appendLog(QString("INFO: Detected new window: %1").arg(xorgWindowId));

        QSize originalSize = x11Window->size();
        windowOriginalSizes.insert(xorgWindowId, originalSize);

        QWidget *containerWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(containerWidget);

        QWidget *windowWidget = QWidget::createWindowContainer(x11Window, containerWidget);
        
        QRect geometry = x11Window->geometry();
        int topbarHeight = 30;

        if (geometry.isValid()) {
            containerWidget->setGeometry(geometry.x(), geometry.y(), geometry.width(), geometry.height() + topbarHeight);
        } else {
            containerWidget->setGeometry(50, 80, 500, 500 + topbarHeight);
        }

        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(windowWidget);
        containerWidget->setLayout(layout);
        containerWidget->show();

        TopBar *topBar = new TopBar(x11Window, this);
        windowTopBars.insert(xorgWindowId, topBar);

        topBar->setGeometry(geometry.x(), geometry.y() - topbarHeight, geometry.width(), topbarHeight);
        topBar->updatePosition();
        topBar->show();

        createCubes(containerWidget, geometry);

        appendLog(QString("INFO: TopBar created for window: %1").arg(xorgWindowId));
    } else {
        appendLog("ERR: Failed to create a window from X11 ID");
    }
}

void WindowManager::createCubes(QWidget *parentWidget, const QRect &geometry) {
    QLabel *leftCube = new QLabel(parentWidget);
    QLabel *rightCube = new QLabel(parentWidget);
    QLabel *bottomCube = new QLabel(parentWidget);

    leftCube->setFixedSize(10, 10);
    rightCube->setFixedSize(10, 10);
    bottomCube->setFixedSize(10, 10);

    leftCube->setStyleSheet("background-color: red;");
    rightCube->setStyleSheet("background-color: blue;");
    bottomCube->setStyleSheet("background-color: green;");

    leftCube->move(geometry.x() - 10, geometry.y() + (geometry.height() / 2) - 5);
    rightCube->move(geometry.x() + geometry.width(), geometry.y() + (geometry.height() / 2) - 5);
    bottomCube->move(geometry.x() + (geometry.width() / 2) - 5, geometry.y() + geometry.height());

    leftCube->show();
    rightCube->show();
    bottomCube->show();
}

void WindowManager::updateTaskbarPosition(QWindow *window) {
    if (windowTopBars.contains(window->winId())) {
        TopBar *topBar = windowTopBars.value(window->winId());
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        
        int topbarHeight = 30;

        QRect windowGeometry = window->geometry();
        window->setGeometry(windowGeometry.x(), windowGeometry.y(), windowGeometry.width(), windowGeometry.height());

        topBar->setGeometry(windowGeometry.x(), windowGeometry.y() - topbarHeight, windowGeometry.width(), topbarHeight);
        topBar->show();
    }
}

void WindowManager::appendLog(const QString &message) {
    QFile logFile("/usr/cydra/logs/a2wm.log");
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ") 
            << message << endl;
        logFile.close();
    }

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
        } else if (mouseEvent->button() == Qt::LeftButton) {
            if (!this->geometry().contains(mouseEvent->pos())) {
                appendLog("INFO: Clicking outside Qt window, refocusing");
                this->activateWindow();
            }
        }
    }

    return QWidget::event(qtEvent);
}

void WindowManager::cleanUpClosedWindows() {
    QList<WId> windowsToRemove;
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
        painter.drawPixmap(0, 0, width(), height(), backgroundPixmap);
    }
}
