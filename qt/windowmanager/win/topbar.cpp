#include "topbar.h"
#include "../windowmanager.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPushButton>
#include <QProcess>
#include <QPainter>
#include <QObject>
#include <QString>

TopBar::TopBar(QWindow *parentWindow, WindowManager *manager, QWidget *parent)
    : QWidget(parent), trackedWindow(parentWindow), isDragging(false) {

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    titleLabel = new QLabel(this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { color: white; }");

    closeButton = new QPushButton("✕", this);
    closeButton->setFixedSize(30, 30);
    connect(closeButton, &QPushButton::clicked, &TopBar::closeTrackedWindow);

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

void TopBar::closeTrackedWindow() {
    if (trackedWindow) {
        WId windowId = trackedWindow->winId();

        QProcess process;
        process.start("xdotool getwindowpid " + QString::number(windowId));
        process.waitForFinished();
        QString pidString = process.readAllStandardOutput().trimmed();
        bool ok;
        qint64 pid = pidString.toLongLong(&ok);

        if (ok && pid > 0) {
            QProcess::execute("kill -9 " + QString::number(pid));
        }
        this->close();
    }
}
