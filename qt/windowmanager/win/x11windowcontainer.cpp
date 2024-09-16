#include "x11windowcontainer.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>

X11WindowContainer::X11WindowContainer(QWindow *window, QWidget *parent)
    : QWidget(parent), x11Window(window) {
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

    Display *display = XOpenDisplay(nullptr);
    if (!display) {
        qWarning() << "Cannot open display";
        return;
    }

    x11WindowId = window->winId();
    Window rootWindow = DefaultRootWindow(display);

    XReparentWindow(display, x11WindowId, winId(), 0, 0);
    XMapWindow(display, x11WindowId);
    XFlush(display);

    XCloseDisplay(display);
}

void X11WindowContainer::updateWindowGeometry() {
    if (x11Window) {
        setGeometry(x11Window->geometry());
    }
}
