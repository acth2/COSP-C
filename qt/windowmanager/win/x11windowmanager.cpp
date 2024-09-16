#include "x11windowmanager.h"
#include <QProcess>
#include <QWindow>

X11WindowManager::X11WindowManager(QObject *parent) : QObject(parent), x11Container(nullptr) {}

void X11WindowManager::startAndEmbedX11Window() {
    QWindow *x11Window = createX11Window();
    x11Container = new X11WindowContainer(x11Window);
    x11Container->show();
}

QWindow* X11WindowManager::createX11Window() {
    QProcess process;
    process.start("xterm");
    process.waitForStarted();
    process.waitForFinished();

    QWindow *x11Window = new QWindow();
    x11Window->setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    return x11Window;
}
