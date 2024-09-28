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
#include <QTransform>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#undef KeyPress

WindowManager::WindowManager(QWidget *parent)
    : QWidget(parent),
      isConsoleVisible(false),
      userInteractRightWidget(nullptr),
      resizeMode(false),
      backgroundImagePath("/usr/cydra/backgrounds/current.png") {

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setSupportingWMCheck();

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

void WindowManager::setSupportingWMCheck() {
    xDisplay = XOpenDisplay(nullptr);
    if (!xDisplay) {
        appendLog("ERR: Failed to open X Display ..");
        return;
    }

    Window supportingWindow = XCreateSimpleWindow(xDisplay, DefaultRootWindow(xDisplay), 0, 0, 1, 1, 0, 0, 0);
    
    Atom netSupportingWMCheck = XInternAtom(xDisplay, "_NET_SUPPORTING_WM_CHECK", False);
    Atom windowId = XInternAtom(xDisplay, "WM_WINDOW", False);
    XChangeProperty(xDisplay, DefaultRootWindow(xDisplay), netSupportingWMCheck, XA_WINDOW, 32, PropModeReplace, (unsigned char *)&supportingWindow, 1);
    
    XMapWindow(xDisplay, supportingWindow);
    XFlush(xDisplay);
    XCloseDisplay(xDisplay);
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
    appendLog(QString("INFO: Creating and tracking window: %1").arg(xorgWindowId));
    
    QWindow *x11Window = QWindow::fromWinId(xorgWindowId);
    
    if (!x11Window) {
        appendLog("ERR: Failed to create QWindow from X11 ID.");
        return;
    }

    trackedWindows.insert(xorgWindowId, x11Window);
    appendLog(QString("INFO: Detected new window: %1").arg(xorgWindowId));

    QWidget *containerWidget = new QWidget(this);
    if (!containerWidget) {
        appendLog("ERR: Failed to create container widget.");
        return;
    }

    QRect geometry = x11Window->geometry();
    int topbarHeight = 30;

    if (geometry.isValid()) {
        containerWidget->setGeometry(geometry.x(), geometry.y(), geometry.width(), geometry.height() + topbarHeight);
    } else {
        containerWidget->setGeometry(50, 80, 400, 400 + topbarHeight);
    }

    QWidget *windowWidget = QWidget::createWindowContainer(x11Window, containerWidget);
    if (!windowWidget) {
        appendLog("ERR: Failed to create window container.");
        return;
    }

    QVBoxLayout *layout = new QVBoxLayout(containerWidget);
    layout->addWidget(windowWidget);

    TopBar *topBar = new TopBar(x11Window, this);
    if (!topBar) {
        appendLog("ERR: Failed to create TopBar.");
        return;
    }

    topBar->setGeometry(containerWidget->geometry().x(), containerWidget->geometry().y() - topbarHeight, 
                        containerWidget->geometry().width(), topbarHeight);
    
    topBar->show();
    containerWidget->show();

    appendLog(QString("INFO: Successfully created container and TopBar for window: %1").arg(xorgWindowId));

    windowTopBars.insert(xorgWindowId, topBar);

    createTrackingSquares(xorgWindowId);

    resizeWindowCubesTimer = new QTimer(this);
    connect(resizeWindowCubesTimer, &QTimer::timeout, this, [this, xorgWindowId, topBar, x11Window]() { 
    if (x11Window) {
        updateTrackingSquares(xorgWindowId);
    } else {
        return;
    }
        topBar->updatePosition();
    });
    resizeWindowCubesTimer->start(1);

    topBar->updatePosition();
}

void WindowManager::createTrackingSquares(WId windowId) {
    Display *display = XOpenDisplay(nullptr);
    if (!display) {
        appendLog("Unable to open X11 display");
        return;
    }

    XWindowAttributes windowAttributes;
    if (!XGetWindowAttributes(display, windowId, &windowAttributes)) {
        appendLog("Unable to get window attributes for windowId: " + QString::number(windowId));
        XCloseDisplay(display);
        return;
    }

    windowGeometry = new QRect(windowAttributes.x, windowAttributes.y, windowAttributes.width, windowAttributes.height);
    int leftSquareWidth = 15;
    int leftSquareHeight = windowGeometry->height();

    QLabel *leftSquare = new QLabel(this);
    leftSquare->setFixedSize(leftSquareWidth, leftSquareHeight);
    leftSquare->setStyleSheet("background-color: red;");
    leftSquare->installEventFilter(this);

    QLabel *rightSquare = new QLabel(this);
    rightSquare->setFixedSize(leftSquareWidth, leftSquareHeight);
    rightSquare->setStyleSheet("background-color: red;");
    rightSquare->installEventFilter(this);

    QLabel *bottomSquare = new QLabel(this);
    bottomSquare->setFixedSize(windowGeometry->width(), 15);
    bottomSquare->setStyleSheet("background-color: red;");
    bottomSquare->installEventFilter(this);

    leftSquare->show();
    rightSquare->show();
    bottomSquare->show();

    TrackingSquares squares = {leftSquare, rightSquare, bottomSquare};
    windowSquares.insert(windowId, squares);

    appendLog(QString("INFO: Created tracking squares for window ID: %1").arg(windowId));

    XCloseDisplay(display);
}

void WindowManager::updateTrackingSquares(WId windowId) {
    if (!windowSquares.contains(windowId)) {
        appendLog("ERR: No tracking squares found for windowId: " + QString::number(windowId));
        return;
    }

    Display *display = XOpenDisplay(nullptr);
    if (!display) {
        appendLog("Unable to open X11 display");
        return;
    }

    XWindowAttributes windowAttributes;
    int result = XGetWindowAttributes(display, windowId, &windowAttributes);
    TrackingSquares squares = windowSquares.value(windowId);

    if (result == 0) {
        appendLog("INFO: Window with ID: " + QString::number(windowId) + " is closed or invalid.");

        squares.leftSquare->hide();
        squares.rightSquare->hide();
        squares.bottomSquare->hide();

        delete squares.leftSquare;
        delete squares.rightSquare;
        delete squares.bottomSquare;

        XCloseDisplay(display);
        return;
    }

    QRect windowGeometry(windowAttributes.x, windowAttributes.y, windowAttributes.width, windowAttributes.height);
    
    squares.leftSquare->move(windowGeometry.left() - squares.leftSquare->width(), windowGeometry.top());
    squares.rightSquare->move(windowGeometry.right(), windowGeometry.top());
    squares.bottomSquare->move(windowGeometry.center().x() - (squares.bottomSquare->width() / 2), windowGeometry.bottom());

    appendLog(QString("INFO: Updated tracking squares for window ID: %1").arg(windowId));

    XCloseDisplay(display);
}

void WindowManager::killTrackingCubes() {
    for (auto it = windowSquares.begin(); it != windowSquares.end(); ++it) {
        TrackingSquares& squares = it.value();
        if (squares.leftSquare) {
            squares.leftSquare->deleteLater();
            squares.leftSquare = nullptr;
        }
        
        if (squares.rightSquare) {
            squares.rightSquare->deleteLater();
            squares.rightSquare = nullptr;
        }
        
        if (squares.bottomSquare) {
            squares.bottomSquare->deleteLater();
            squares.bottomSquare = nullptr;
        }
    }
    windowSquares.clear();
}

void WindowManager::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && resizeMode) {
        resizeMode = false;
    }
    return QWidget::mouseReleaseEvent(event);
}

bool WindowManager::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            for (const auto &squares : windowSquares) {
                if (object == squares.leftSquare || object == squares.rightSquare || object == squares.bottomSquare) {
                    resizeMode = true;
                    lastMousePosition = mouseEvent->globalPos();
                    int leftY = squares.leftSquare->y();
                    int rightY = squares.rightSquare->y();
                    int newWidth = 2500;
                    int newHeight = windowGeometry->height();

                    squares.leftSquare->setGeometry(squares.leftSquare->x() - 1000, leftY, newWidth, newHeight);
                    squares.rightSquare->setGeometry(squares.rightSquare->x() - 1000, rightY, newWidth, newHeight);

                    squares.bottomSquare->setGeometry(squares.bottomSquare->x(), squares.bottomSquare->y(), windowGeometry->width(), 2500);
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(object, event);
}

void WindowManager::mouseMoveEvent(QMouseEvent *event) {
    if (resizeMode) {
        QPoint currentPos = event->globalPos();
        int dx = currentPos.x() - lastMousePosition.x();
        int dy = currentPos.y() - lastMousePosition.y();

        for (auto windowId : trackedWindows.keys()) {
            TrackingSquares squares = windowSquares.value(windowId);
            
            if (squares.leftSquare->geometry().contains(event->pos())) {
                QRect newGeometry = trackedWindows[windowId]->geometry();
                newGeometry.setLeft(newGeometry.left() + dx);
                trackedWindows[windowId]->setGeometry(newGeometry);
            } else if (squares.rightSquare->geometry().contains(event->pos())) {
                QRect newGeometry = trackedWindows[windowId]->geometry();
                newGeometry.setRight(newGeometry.right() + dx);
                trackedWindows[windowId]->setGeometry(newGeometry);
            } else if (squares.bottomSquare->geometry().contains(event->pos())) {
                QRect newGeometry = trackedWindows[windowId]->geometry();
                newGeometry.setBottom(newGeometry.bottom() + dy);
                trackedWindows[windowId]->setGeometry(newGeometry);
            }
        }

        lastMousePosition = currentPos;
    }
}

void WindowManager::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void WindowManager::closeWindow(WId windowId) {
    if (trackedWindows.contains(windowId)) {
        QWindow* window = trackedWindows.value(windowId);
        if (window) {
            window->hide();
            trackedWindows.remove(windowId);
            appendLog("INFO: Window killed");
        }
    }
}

void WindowManager::updateTaskbarPosition(QWindow *window) {
    if (windowTopBars.contains(window->winId())) {
        TopBar *topBar = windowTopBars.value(window->winId());
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        
        int windowWidth = window->width();
        int windowHeight = window->height();
        int topbarHeight = 30;

        int centeredX = (screenGeometry.width() - windowWidth) / 2;
        int centeredY = (screenGeometry.height() - windowHeight) / 2;

        if (windowWidth <= 0 || windowHeight <= 0) {
            windowWidth = 800;
            windowHeight = 600;
            centeredX = (screenGeometry.width() - windowWidth) / 2;
            centeredY = (screenGeometry.height() - windowHeight) / 2;
        }

        window->setGeometry(centeredX, centeredY, windowWidth, windowHeight);

        topBar->setGeometry(centeredX, centeredY - topbarHeight, windowWidth, topbarHeight);
        topBar->show();
    }
}

void WindowManager::appendLog(const QString &message) {
    QFile logFile("/usr/cydra/logs/cwm.log");
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
