#include "windowmanager.h"
#include "win/topbar.h"
#include "win/utils/closebutton.h"
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
#include <QDateTime>
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
    if (xDisplay) {
        Atom netWmWindowType = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE", False);
        Atom netWmWindowTypeNormal = XInternAtom(xDisplay, "_NET_WM_WINDOW_TYPE_NORMAL", False);

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
                        if (atoms[0] != netWmWindowTypeNormal) {
                            XFree(data);
                            continue;
                        }
                        XFree(data);
                    }
                }

                XWindowAttributes attributes;
                if (XGetWindowAttributes(xDisplay, child, &attributes) == 0 || attributes.map_state != IsViewable) {
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
    QWindow *window = QWindow::fromWinId(xorgWindowId);
    if (window) {
        trackedWindows.insert(xorgWindowId, window);
        appendLog(QString("INFO: Detected new window: %1").arg(xorgWindowId));

        QTimer::singleShot(500, this, [this, xorgWindowId, window]() {
            QRect geometry = window->geometry();

            if (geometry.width() == 0 || geometry.height() == 0) {
                appendLog("WARN: Still zero-size window after delay: " + QString::number(xorgWindowId));
                window->setGeometry(50, 50, 500, 500);
                appendLog("INFO: Setting window to 500x500 dim.");
            }

            
            TopBar *topBar = new TopBar(window, this);
            windowTopBars.insert(xorgWindowId, topBar);
            
            topBar->updatePosition();

            connect(topBar, &TopBar::closeRequested, [window, this, xorgWindowId]() {
                appendLog("INFO: Window closed: " + QString::number(xorgWindowId));
                window->hide();
                windowTopBars.remove(xorgWindowId);
            });
        });
    }
}

void WindowManager::setupCloseButton(QWindow *window) {
    CloseButton *closeButton = new CloseButton(window, this);
    windowCloseButtons.insert(window->winId(), closeButton);
    closeButton->updatePosition();
    appendLog("INFO: CloseButton created and positioned for window: " + QString::number(window->winId()));
}

void WindowManager::removeCloseButton(WId windowId) {
    if (windowCloseButtons.contains(windowId)) {
        CloseButton *closeButton = windowCloseButtons.value(windowId);
        closeButton->deleteLater();
        windowCloseButtons.remove(windowId);
    }
}

void WindowManager::closeWindow(WId xorgWindowId) {
    if (trackedWindows.contains(xorgWindowId)) {
        trackedWindows[xorgWindowId]->close();
        trackedWindows.remove(xorgWindowId);
    }

    if (windowTopBars.contains(xorgWindowId)) {
        windowTopBars[xorgWindowId]->deleteLater();
        windowTopBars.remove(xorgWindowId);
    }

    if (closeButton) {
        closeButton->deleteLater();
        closeButton = nullptr;
        appendLog("INFO: Close button removed for window: " + QString::number(xorgWindowId));
    } else {
        appendLog("WARN: No Close button to remove for window: " + QString::number(xorgWindowId));
    }

    appendLog(QString("INFO: Closed and removed window: %1").arg(xorgWindowId));
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

        removeCloseButton(xorgWindowId);
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
