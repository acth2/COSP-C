#include "topbar.h"
#include <QtCore/qtextstream.h>
#include "../windowmanager.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPushButton>
#include <QProcess>
#include <QPainter>
#include <QString>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#ifdef None
#undef None
#endif

#ifdef Status
#undef Status
#endif

#ifdef Success
#undef Success
#endif

TopBar::TopBar(QWindow *parentWindow, WindowManager *manager, QWidget *parent)
    : QWidget(parent), trackedWindow(parentWindow), isDragging(false) {

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    titleLabel = new QLabel(this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { color: white; }");

    closeButton = new QPushButton("✕", this);
    closeButton->setFixedSize(30, 30);
    connect(closeButton, &QPushButton::clicked, [this]() {
        if (trackedWindow) {
            pid_t pid = getProcessIdFromWindow(trackedWindow->winId());
        
            if (pid > 0) {
                QString command = QString("kill -9 %1").arg(pid);
                QProcess::execute(command);
            }
            this->close();
        }
    });

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addStretch();
    layout->addWidget(closeButton);
    layout->setContentsMargins(10, 5, 10, 2);
    setLayout(layout);
    
    updatePosition();
}

void TopBar::updatePosition() {
    if (trackedWindow) {
        QRect windowGeometry = trackedWindow->geometry();
        int topbarHeight = 36;
        setGeometry(windowGeometry.x(), windowGeometry.y() - topbarHeight, windowGeometry.width(), topbarHeight);
        show();
    }
}


pid_t getProcessIdFromWindow(WId windowId) {
    Display *display = XOpenDisplay(nullptr);
    if (!display) {
        return -1;
    }

    pid_t pid = -1;
    Atom atom = XInternAtom(display, "_NET_WM_PID", False);
    if (atom != None) {
        Atom type;
        int format;
        unsigned long nitems, bytesafter;
        unsigned char *data = nullptr;

        int status = XGetWindowProperty(display, windowId, atom, 0, sizeof(pid_t), False, XA_CARDINAL,
                                        &type, &format, &nitems, &bytesafter, &data);
        if (status == Success && data) {
            if (format == 32 && nitems >= 1) {
                pid = static_cast<pid_t>(reinterpret_cast<pid_t*>(data)[0]);
            }
            XFree(data);
        }
    }

    XCloseDisplay(display);
    return pid;
}

void TopBar::updateTitle(const QString &title) {
    titleLabel->setText(title);
}

void TopBar::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(0, 0, 0, 150));
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());
}

void TopBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        dragStartPos = event->globalPos();
        windowStartPos = trackedWindow->geometry().topLeft();
    }
}

void TopBar::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        QPoint delta = event->globalPos() - dragStartPos;
        QPoint newWindowPos = windowStartPos + delta;
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        trackedWindow->setGeometry(QRect(newWindowPos, trackedWindow->geometry().size()));
        updatePosition();
    }
}

void TopBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        QApplication::setOverrideCursor(Qt::ArrowCursor);
    }
}
