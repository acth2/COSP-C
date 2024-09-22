#include "topbar.h"
#include "../windowmanager.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPushButton>
#include <QProcess>
#include <QPainter>
#include <QObject>
#include <QString>
#include <QGuiApplication>
#include <QScreen>
#include <QFile>

TopBar::TopBar(QWindow *parentWindow, WindowManager *manager, QWidget *parent)
    : QWidget(parent), trackedWindow(parentWindow), isDragging(false) {

    if (QFile::exists("/usr/cydra/settings/darkmode")) {
        isDarkMode = true;
    } else {
        isDarkMode = false;
    }
        
    trackedWindow->installEventFilter(this);
    isMaximized = false;

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_TranslucentBackground);

    titleLabel = new QLabel(this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { color: white; }");

    popup = new QLabel(this);
    popup->setFixedSize(500, 500);
    //popup->setStyleSheet("background-color: #333;");
        
    closeButton = new QPushButton("✕", this);
    closeButton->setFixedSize(30, 30);
    if (!isDarkMode){
        closeButton->setStyleSheet(
            "QPushButton {"
            "   border-radius: 15px;"
            "   background-color: black;"
            "   color: white;"
            "   border: none;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background-color: red;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #8B0000;"
            "}"
        );
    } else {
        closeButton->setStyleSheet(
            "QPushButton {"
            "   border-radius: 15px;"
            "   background-color: white;"
            "   color: black;"
            "   border: none;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background-color: red;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #8B0000;"
            "}"
        );
    }

    maximizeButton = new QPushButton(this);
    maximizeButton->setText("❐");
    maximizeButton->setFixedSize(30, 30);
    if (!isDarkMode){
        maximizeButton->setStyleSheet(
            "QPushButton {"
            "   background-color: black;"
            "   border-radius: 15px;"
            "   font-size: 18px;"
            "}"
            "QPushButton:hover {"
            "   background-color: gray;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #404040;"
            "}"
        );
    } else {
        maximizeButton->setStyleSheet(
            "QPushButton {"
            "   background-color: white;"
            "   border-radius: 15px;"
            "   font-size: 18px;"
            "}"
            "QPushButton:hover {"
            "   background-color: gray;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #404040;"
            "}"
        );
    }

    connect(closeButton, &QPushButton::clicked, this, &TopBar::closeTrackedWindow);
    connect(maximizeButton, &QPushButton::clicked, this, &TopBar::toggleMaximizeRestore);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addStretch();
    layout->addWidget(maximizeButton);
    layout->addWidget(closeButton);
    layout->setContentsMargins(10, 5, 10, 2);

    setLayout(layout);

    leftResizeHandle = new QWindow();
    rightResizeHandle = new QWindow();
    bottomResizeHandle = new QWindow();

    setupResizeHandle(leftResizeHandle, Qt::LeftEdge);
    setupResizeHandle(rightResizeHandle, Qt::RightEdge);
    setupResizeHandle(bottomResizeHandle, Qt::BottomEdge);
    updatePosition();
}


QWindow* TopBar::getTrackedWindow() const {
    return trackedWindow;
}

QLabel* TopBar::getPopup() const {
    return popup;
}

void TopBar::createResizeHandles() {
    rightResizeHandle = new QWindow();
    rightResizeHandle->setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::BypassWindowManagerHint);
    rightResizeHandle->setGeometry(QRect(0, 0, 10, trackedWindow->height()));
    rightResizeHandle->show();

    bottomResizeHandle = new QWindow();
    bottomResizeHandle->setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::BypassWindowManagerHint);
    bottomResizeHandle->setGeometry(QRect(0, 0, trackedWindow->width(), 10));
    bottomResizeHandle->show();

    leftResizeHandle = new QWindow();
    leftResizeHandle->setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::BypassWindowManagerHint);
    leftResizeHandle->setGeometry(QRect(0, 0, 10, trackedWindow->height()));
    leftResizeHandle->show();

    updateResizeHandles();
}

void TopBar::setupResizeHandle(QWindow *handle, Qt::Edge edge) {
    handle->setGeometry(0, 0, 10, 10);
    handle->installEventFilter(this);

    if (edge == Qt::RightEdge || edge == Qt::LeftEdge) {
        handle->setCursor(Qt::SizeHorCursor);
    } else if (edge == Qt::BottomEdge) {
        handle->setCursor(Qt::SizeVerCursor);
    }
}

bool TopBar::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (obj == leftResizeHandle) {
            isResizing = true;
            resizeEdge = Qt::LeftEdge;
            resizeStartPos = mouseEvent->globalPos();
            initialWindowGeometry = trackedWindow->geometry();
        } else if (obj == rightResizeHandle) {
            isResizing = true;
            resizeEdge = Qt::RightEdge;
            resizeStartPos = mouseEvent->globalPos();
            initialWindowGeometry = trackedWindow->geometry();
        } else if (obj == bottomResizeHandle) {
            isResizing = true;
            resizeEdge = Qt::BottomEdge;
            resizeStartPos = mouseEvent->globalPos();
            initialWindowGeometry = trackedWindow->geometry();
        }
        return true;
    } else if (event->type() == QEvent::MouseMove && isResizing) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint delta = mouseEvent->globalPos() - resizeStartPos;
        QRect newGeometry = initialWindowGeometry;

        if (resizeEdge == Qt::RightEdge) {
            newGeometry.setWidth(initialWindowGeometry.width() + delta.x());
        } else if (resizeEdge == Qt::LeftEdge) {
            newGeometry.setLeft(initialWindowGeometry.left() + delta.x());
        } else if (resizeEdge == Qt::BottomEdge) {
            newGeometry.setHeight(initialWindowGeometry.height() + delta.y());
        }

        trackedWindow->setGeometry(newGeometry);
        updateResizeHandles();
        return true;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        isResizing = false;
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void TopBar::updateResizeHandles() {
    if (!trackedWindow) return;

    QRect windowGeometry = trackedWindow->geometry();
    leftResizeHandle->setGeometry(windowGeometry.left() - 5, windowGeometry.top(), 10, windowGeometry.height());
    rightResizeHandle->setGeometry(windowGeometry.right() - 5, windowGeometry.top(), 10, windowGeometry.height());
    bottomResizeHandle->setGeometry(windowGeometry.left(), windowGeometry.bottom() - 5, windowGeometry.width(), 10);
}

void TopBar::updatePosition() {
    if (!trackedWindow) return;

    QRect windowGeometry = trackedWindow->geometry();
    move(windowGeometry.left(), windowGeometry.top() - height());

    updateResizeHandles();
}

void TopBar::updateTitle(const QString &title) {
    titleLabel->setText(title);
}

void TopBar::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (!isDarkMode) {
        painter.setBrush(QColor(255, 255, 255, 150));    
    } else {
        painter.setBrush(QColor(51, 51, 51, 150));
    }
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());
}

void TopBar::mousePressEvent(QMouseEvent *event) {
    resizeStartPos = event->globalPos();
    if (rightResizeHandle->geometry().contains(event->pos())) {
        resizingRight = true;
        setCursor(Qt::SizeHorCursor);
    } else if (leftResizeHandle->geometry().contains(event->pos())) {
        resizingLeft = true;
        setCursor(Qt::SizeHorCursor);
    } else if (bottomResizeHandle->geometry().contains(event->pos())) {
        resizingBottom = true;
        setCursor(Qt::SizeVerCursor);
    } else {
        isDragging = true;
        dragStartPos = event->globalPos();
        windowStartPos = trackedWindow->position();
        setCursor(Qt::ClosedHandCursor);
    }
    updatePosition();
    QWidget::mousePressEvent(event);
}

void TopBar::mouseReleaseEvent(QMouseEvent *event) {
    resizingRight = false;
    resizingLeft = false;
    resizingBottom = false;
    isDragging = false;
    setCursor(Qt::ArrowCursor);
    QWidget::mouseReleaseEvent(event);
    updatePosition();
}

void TopBar::mouseMoveEvent(QMouseEvent *event) {
    if (resizingRight) {
        handleResizeRight(event->globalPos());
    } else if (resizingLeft) {
        handleResizeLeft(event->globalPos());
    } else if (resizingBottom) {
        handleResizeBottom(event->globalPos());
    } else if (isDragging) {
        if (isMaximized) {
            trackedWindow->setGeometry(restoreGeometry);
            isMaximized = false;
        }
        trackedWindow->setPosition(windowStartPos + (event->globalPos() - dragStartPos));
    }
    updatePosition();
    QWidget::mouseMoveEvent(event);
}

void TopBar::handleResizeRight(const QPoint &mousePos) {
    int deltaX = mousePos.x() - resizeStartPos.x();
    QRect windowGeometry = trackedWindow->geometry();
    if (windowGeometry.width() + deltaX >= minimumWidth()) {
        trackedWindow->setGeometry(windowGeometry.x(), windowGeometry.y(), windowGeometry.width() + deltaX, windowGeometry.height());
    }
    resizeStartPos = mousePos;
}

void TopBar::handleResizeLeft(const QPoint &mousePos) {
    int deltaX = mousePos.x() - resizeStartPos.x();
    QRect windowGeometry = trackedWindow->geometry();
    if (windowGeometry.width() - deltaX >= minimumWidth()) {
        trackedWindow->setGeometry(windowGeometry.x() + deltaX, windowGeometry.y(), windowGeometry.width() - deltaX, windowGeometry.height());
    }
    resizeStartPos = mousePos;
}

void TopBar::handleResizeBottom(const QPoint &mousePos) {
    int deltaY = mousePos.y() - resizeStartPos.y();
    QRect windowGeometry = trackedWindow->geometry();
    if (windowGeometry.height() + deltaY >= minimumHeight()) {
        trackedWindow->setGeometry(windowGeometry.x(), windowGeometry.y(), windowGeometry.width(), windowGeometry.height() + deltaY);
    }
    resizeStartPos = mousePos;
}

void TopBar::closePopup() {
    if (popup->isVisible()) {
        popup->hide();
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

void TopBar::toggleMaximizeRestore() {
    if (isMaximized) {
        trackedWindow->setGeometry(restoreGeometry);
        isMaximized = false;
    } else {
        restoreGeometry = trackedWindow->geometry();
        maximizeWindow();
        isMaximized = true;
    }

    updatePosition();
}

void TopBar::maximizeWindow() {
    if (trackedWindow) {
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->availableGeometry();

        int topbarHeight = 36;
        int bottomMargin = 40;

        int newWidth = screenGeometry.width();
        int newHeight = screenGeometry.height() - bottomMargin - topbarHeight;

        trackedWindow->setGeometry(0, topbarHeight, newWidth, newHeight);

        updatePosition();
    }
}
