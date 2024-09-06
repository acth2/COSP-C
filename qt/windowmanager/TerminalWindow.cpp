#include "TerminalWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QTextEdit>
#include <QPushButton>
#include <QDebug>
#include <QScreen>
#include <QCursor>

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), isFullScreenMode(false), dragging(false), resizing(false) {
    setupUI();
    setMouseTracking(true); 
}

void TerminalWindow::mouseMoveEvent(QMouseEvent *event) {
    const int resizeMargin = 8;
    QRect windowRect = rect();

    bool nearLeft = event->pos().x() <= resizeMargin;
    bool nearRight = event->pos().x() >= windowRect.width() - resizeMargin;
    bool nearTop = event->pos().y() <= resizeMargin;
    bool nearBottom = event->pos().y() >= windowRect.height() - resizeMargin;

    if (nearLeft || nearRight || nearTop || nearBottom) {
        if (nearLeft && nearBottom) {
            setCursor(Qt::SizeBDiagCursor);  
        } else if (nearRight && nearBottom) {
            setCursor(Qt::SizeFDiagCursor);  
        } else if (nearLeft && nearTop) {
            setCursor(Qt::SizeFDiagCursor); 
        } else if (nearRight && nearTop) {
            setCursor(Qt::SizeBDiagCursor); 
        } else if (nearLeft || nearRight) {
            setCursor(Qt::SizeHorCursor); 
        } else if (nearTop || nearBottom) {
            setCursor(Qt::SizeVerCursor);  
        }
        resizing = true;
    } else {
        setCursor(Qt::ArrowCursor); 
        resizing = false;
    }

    if (resizing && (event->buttons() & Qt::LeftButton)) {
        QRect newGeometry = geometry();

        if (nearRight) {
            newGeometry.setWidth(event->globalX() - frameGeometry().left());
        } else if (nearLeft) {
            int diff = event->globalX() - frameGeometry().left();
            newGeometry.setLeft(event->globalX());
            newGeometry.setWidth(newGeometry.width() - diff);
        }
        if (nearBottom) {
            newGeometry.setHeight(event->globalY() - frameGeometry().top());
        } else if (nearTop) {
            int diff = event->globalY() - frameGeometry().top();
            newGeometry.setTop(event->globalY());
            newGeometry.setHeight(newGeometry.height() - diff);
        }

        setGeometry(newGeometry);
    }

    QMainWindow::mouseMoveEvent(event);
}

void TerminalWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = !resizing;
        dragStartPosition = event->globalPos() - frameGeometry().topLeft();
    }
    QMainWindow::mousePressEvent(event);
}

void TerminalWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = false;
        resizing = false;
    }
    QMainWindow::mouseReleaseEvent(event);
}
