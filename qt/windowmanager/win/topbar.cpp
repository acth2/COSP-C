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
    : QWidget(parent), trackedWindow(parentWindow), isDragging(false), windowManager(manager) {

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::ClickFocus);

    titleLabel = new QLabel(this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { color: white; }");

    closeButton = new QPushButton("✕", this);
    closeButton->setFixedSize(30, 30);
    connect(closeButton, &QPushButton::clicked, this, &TopBar::closeTrackedWindow);

    popup = new QLabel(this);
    popup->setFixedSize(500, 500);
    popup->setStyleSheet("background-color: #333;");
    popup->hide();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addStretch();
    layout->addWidget(closeButton);
    layout->setContentsMargins(10, 5, 10, 2);
    setLayout(layout);

    updatePosition();
}

void TopBar::closePopup() {
    if (popup->isVisible()) {
        popup->hide();
    }
}

QLabel* TopBar::getPopup() const {
    return popup;
}

QWindow* TopBar::getTrackedWindow() const {
    return trackedWindow;
}

void TopBar::mousePressEvent(QMouseEvent *event) {
    if (trackedWindow) {
        QMouseEvent *forwardedEvent = new QMouseEvent(
            event->type(), event->pos(), event->button(), event->buttons(), event->modifiers());
        QApplication::sendEvent(trackedWindow, forwardedEvent);
    }
    QWidget::mousePressEvent(event);
}

void TopBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        QApplication::setOverrideCursor(Qt::ArrowCursor);

        if (trackedWindow) {
            trackedWindow->requestActivate();
        }
    }
}

void TopBar::mouseMoveEvent(QMouseEvent *event) {
    if (trackedWindow) {
        QMouseEvent *forwardedEvent = new QMouseEvent(
            event->type(), event->pos(), event->button(), event->buttons(), event->modifiers());
        QApplication::sendEvent(trackedWindow, forwardedEvent);
    }
    QWidget::mouseMoveEvent(event);
}

bool TopBar::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        if (trackedWindow && this->geometry().contains(mouseEvent->pos())) {
            trackedWindow->requestActivate();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void TopBar::closeTrackedWindow() {
    if (trackedWindow) {
        trackedWindow->close();
        emit closeRequested();
    }
}

void TopBar::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0, 0, 0, 160));
}
