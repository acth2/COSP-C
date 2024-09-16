#include "x11windowcontainer.h"
#include <QX11Info>
#include <QX11EmbedContainer>

X11WindowContainer::X11WindowContainer(QWindow *window, QWidget *parent)
    : QWidget(parent), x11Window(window) {
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setGeometry(window->geometry());
    QX11Info::setDisplay(QX11Info::display());

    QWidget *embedWidget = new QX11EmbedContainer(this);
    embedWidget->setAttribute(Qt::WA_TranslucentBackground);
    embedWidget->setGeometry(this->geometry());

    embedWidget->winId();
}

void X11WindowContainer::updateWindowGeometry() {
    if (x11Window) {
        setGeometry(x11Window->geometry());
    }
}
