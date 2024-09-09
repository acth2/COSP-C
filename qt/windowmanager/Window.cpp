#include "window.h"
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>

Window::Window(Display *disp, Window root, QWidget *parent)
    : QWidget(parent), display(disp), rootWindow(root), dragging(false), isFullScreenMode(false) {
    layout = new QVBoxLayout(this);
    setLayout(layout);
    addTopBar();

    detectWindows();
}

Window::~Window() {
    XCloseDisplay(display);
}

void Window::detectWindows() {
    Window root_return, parent_return;
    Window *children;
    unsigned int nchildren;

    if (XQueryTree(display, rootWindow, &root_return, &parent_return, &children, &nchildren) == 0) {
        return;
    }

    for (unsigned int i = 0; i < nchildren; i++) {
        attachToWindow(children[i]);
    }

    XFree(children);
}

void Window::attachToWindow(Window target) {
    targetWindow = target;

    XWindowAttributes attributes;
    XGetWindowAttributes(display, targetWindow, &attributes);

    int barHeight = 30;
    int newHeight = attributes.height - barHeight;

    XMoveResizeWindow(display, targetWindow, attributes.x, attributes.y + barHeight, attributes.width, newHeight);

    sendConfigureEvent(targetWindow, attributes.x, attributes.y + barHeight, attributes.width, newHeight);
}

void Window::addTopBar() {
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

    layout->addWidget(topBar);

    connect(closeButton, &QPushButton::clicked, this, &Window::closeWindow);
    connect(fullscreenButton, &QPushButton::clicked, this, &Window::toggleFullScreen);
}

void Window::closeWindow() {
    if (targetWindow) {
        XDestroyWindow(display, targetWindow);
    }
    close();
}

void Window::toggleFullScreen() {
    if (isFullScreenMode) {
        showNormal();
        isFullScreenMode = false;
    } else {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry);
        showFullScreen();
        isFullScreenMode = true;
    }
}

void Window::mousePressEvent(QMouseEvent *event) {
    dragging = true;
    dragStartPosition = event->globalPos() - frameGeometry().topLeft();
}

void Window::mouseMoveEvent(QMouseEvent *event) {
    if (dragging) {
        move(event->globalPos() - dragStartPosition);
    }
}

void Window::mouseReleaseEvent(QMouseEvent *event) {
    dragging = false;
}

void Window::resizeEvent(QResizeEvent *event) {
    // Handle resizing logic
}

void Window::sendConfigureEvent(Window win, int x, int y, int width, int height) {
    XConfigureEvent configureEvent;
    configureEvent.type = ConfigureNotify;
    configureEvent.display = display;
    configureEvent.event = win;
    configureEvent.window = win;
    configureEvent.x = x;
    configureEvent.y = y;
    configureEvent.width = width;
    configureEvent.height = height;
    configureEvent.border_width = 0;
    configureEvent.above = None;
    configureEvent.override_redirect = False;

    XSendEvent(display, win, False, StructureNotifyMask, (XEvent*)&configureEvent);
    XFlush(display);
}
