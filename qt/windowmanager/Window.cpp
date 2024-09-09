#include "Window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QCursor>
#include <QMouseEvent>
#include <QX11Info>
#include <X11/Xlib.h>
#include <QDebug>

Window::Window(QWidget *parent)
    : QMainWindow(parent), isFullScreenMode(false), dragging(false), resizing(false) {
    monitorWindows();
}

void Window::monitorWindows() {
    Display *display = QX11Info::display();
    Window root = DefaultRootWindow(display);

    Window rootReturn, parentReturn;
    Window *children;
    unsigned int numChildren;

    if (XQueryTree(display, root, &rootReturn, &parentReturn, &children, &numChildren)) {
        for (unsigned int i = 0; i < numChildren; ++i) {
            WId windowId = children[i];
            XWindowAttributes attributes;
            if (XGetWindowAttributes(display, windowId, &attributes) && attributes.map_state == IsViewable) {
                setupUI(windowId);
            }
        }
        XFree(children);
    }
}

void Window::setupUI(WId targetWinId) {
    currentWinId = targetWinId;

    // Setup custom top bar and layout
    topBar = new QWidget(this);
    topBar->setFixedHeight(30);
    topBar->setStyleSheet("background-color: #333; color: white;");
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(0, 0, 0, 0);

    closeButton = new QPushButton("✕", topBar);
    fullscreenButton = new QPushButton("❐", topBar);

    topBarLayout->addWidget(fullscreenButton);
    topBarLayout->addStretch();
    topBarLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, [this]() { close(); });
    connect(fullscreenButton, &QPushButton::clicked, [this]() { toggleFullScreen(); });

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(topBar);
    setLayout(mainLayout);

    moveWindow(targetWinId);
    resizeWindow(targetWinId);
}

void Window::moveWindow(WId targetWinId) {
    connect(topBar, &QWidget::mousePressEvent, this, [this, targetWinId](QMouseEvent *event) {
        if (event->button() == Qt::LeftButton) {
            dragging = true;
            dragStartPosition = event->globalPos();
        }
    });

    connect(topBar, &QWidget::mouseMoveEvent, this, [this, targetWinId](QMouseEvent *event) {
        if (dragging) {
            QPoint delta = event->globalPos() - dragStartPosition;
            dragStartPosition = event->globalPos();

            XMoveWindow(QX11Info::display(), targetWinId, delta.x(), delta.y());
        }
    });

    connect(topBar, &QWidget::mouseReleaseEvent, this, [this]() {
        dragging = false;
    });
}

void Window::resizeWindow(WId targetWinId) {
    // Not implemented for now
    // Maybe soon maybe maybe...
}

void Window::toggleFullScreen() {
    if (isFullScreenMode) {
        isFullScreenMode = false;
    } else {
        isFullScreenMode = true;
    }
}
