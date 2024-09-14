#include "topbar.h"
#include <QPainter>
#include <QScreen>
#include <QApplication>
#include <QMouseEvent>

TopBar::TopBar(QWindow *parentWindow, QWidget *parent)
    : QWidget(parent), trackedWindow(parentWindow), isDragging(false) {

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    titleLabel = new QLabel(this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { color: white; }");

    closeButton = new QPushButton("✕", this);
    closeButton->setFixedSize(20, 20);
    closeButton->setStyleSheet("QPushButton { background-color: darkred; color: white; border: none; border-radius: 10px; }"
                               "QPushButton:hover { background-color: red; }");
    connect(closeButton, &QPushButton::clicked, this, &TopBar::onCloseButtonClicked);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addStretch();
    layout->addWidget(closeButton);
    layout->setContentsMargins(10, 5, 10, 5);
    setLayout(layout);
    updatePosition();
}

void TopBar::updatePosition() {
    if (trackedWindow) {
        QRect windowGeometry = trackedWindow->geometry();
        int topbarHeight = 30;
        setGeometry(windowGeometry.x(), windowGeometry.y() - topbarHeight, windowGeometry.width(), topbarHeight);
        setStyleSheet("background-color: rgba(0, 0, 0, 150);");
        show();
    }
}

void TopBar::updateTitle(const QString &title) {
    titleLabel->setText(title);
}

void TopBar::onCloseButtonClicked() {
    if (trackedWindow) {
        trackedWindow->close();
    }
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
