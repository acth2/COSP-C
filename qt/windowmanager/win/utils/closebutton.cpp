#include "closebutton.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

CloseButton::CloseButton(QWindow *parentWindow, QWidget *parent)
    : QWidget(parent), trackedWindow(parentWindow)
{
    setFixedSize(30, 30);
    setStyleSheet("background-color: rgba(255, 0, 0, 100);");
}

void CloseButton::enterEvent(QEvent *event)
{
    setStyleSheet("background-color: rgba(255, 0, 0, 200);");
}

void CloseButton::leaveEvent(QEvent *event)
{
    setStyleSheet("background-color: rgba(255, 0, 0, 100);");
}

void CloseButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit closeButtonClicked();

        trackedWindow->close();
        parentWidget()->close();
        close();
    }
}
