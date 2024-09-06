#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XTest.h>
#include <iostream>
#include <QApplication>
#include "taskbar.h"

class GlobalKeyListener : public QObject {
    Q_OBJECT

public:
    GlobalKeyListener(TaskBar *taskbar, QObject *parent = nullptr) 
        : QObject(parent), taskbar(taskbar), display(XOpenDisplay(nullptr)) {}

    ~GlobalKeyListener() {
        if (display) {
            XCloseDisplay(display);
        }
    }

    bool startListening() {
        if (!display) {
            return false;
        }

        XEvent event;
        while (true) {
            XNextEvent(display, &event);

            if (event.type == KeyPress) {
                KeySym key = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);
                if (key == XK_Super_L || key == XK_Super_R) {
                    taskbar->showPopup();
                }
            }
        }
        return true;
    }

private:
    TaskBar *taskbar;
    Display *display;
};
