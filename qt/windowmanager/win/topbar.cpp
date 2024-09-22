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
#include <QCursor>
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
            "   color: white;"
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

    updatePosition();
}

QWindow* TopBar::getTrackedWindow() const {
    return trackedWindow;
}

QLabel* TopBar::getPopup() const {
    return popup;
}

bool TopBar::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        if (getTrackedWindow()) {
            getTrackedWindow()->requestActivate();
            getTrackedWindow()->raise();
            this->raise();
        }

        if (getPopup()->isVisible() && !getPopup()->geometry().contains(mouseEvent->globalPos())) {
            closePopup();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
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
    if (!isDarkMode) {
        painter.setBrush(QColor(255, 255, 255, 150));    
    } else {
        painter.setBrush(QColor(51, 51, 51, 150));
    }
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());
}


enum ResizeRegion {
    NoResize,
    TopEdge,
    BottomEdge,
    LeftEdge,
    RightEdge,
    TopLeftCorner,
    TopRightCorner,
    BottomLeftCorner,
    BottomRightCorner
};

ResizeRegion currentResizeRegion = NoResize;

void TopBar::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        QPoint delta = event->globalPos() - dragStartPos;
        QPoint newWindowPos = windowStartPos + delta;
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        trackedWindow->setGeometry(QRect(newWindowPos, trackedWindow->geometry().size()));
        updatePosition();
    } else {
        detectEdge(event->pos());
        if (currentResizeRegion != NoResize) {
            QApplication::setOverrideCursor(cursorForResizeRegion(currentResizeRegion));
        } else {
            QApplication::restoreOverrideCursor();
        }
    }
}

void TopBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (currentResizeRegion != NoResize) {
            isDragging = true;
            dragStartPos = event->globalPos();
            windowStartPos = trackedWindow->geometry().topLeft();
            startResize();
        } else {
            isDragging = true;
            dragStartPos = event->globalPos();
            windowStartPos = trackedWindow->geometry().topLeft();
            if (trackedWindow) {
                trackedWindow->requestActivate();
            }
        }
    }
}

void TopBar::startResize() {
    QRect geometry = trackedWindow->geometry();
    QRect newGeometry = geometry;

    QPoint globalMousePos = QCursor::pos();
    QPoint delta = globalMousePos - dragStartPos;

    switch (currentResizeRegion) {
        case TopEdge:
            newGeometry.setTop(geometry.top() + delta.y());
            break;
        case BottomEdge:
            newGeometry.setBottom(geometry.bottom() + delta.y());
            break;
        case LeftEdge:
            newGeometry.setLeft(geometry.left() + delta.x());
            break;
        case RightEdge:
            newGeometry.setRight(geometry.right() + delta.x());
            break;
        case TopLeftCorner:
            newGeometry.setTop(geometry.top() + delta.y());
            newGeometry.setLeft(geometry.left() + delta.x());
            break;
        case TopRightCorner:
            newGeometry.setTop(geometry.top() + delta.y());
            newGeometry.setRight(geometry.right() + delta.x());
            break;
        case BottomLeftCorner:
            newGeometry.setBottom(geometry.bottom() + delta.y());
            newGeometry.setLeft(geometry.left() + delta.x());
            break;
        case BottomRightCorner:
            newGeometry.setBottom(geometry.bottom() + delta.y());
            newGeometry.setRight(geometry.right() + delta.x());
            break;
        default:
            break;
    }

    if (trackedWindow) {
        trackedWindow->setGeometry(newGeometry);
        updatePosition();
    }
}

void TopBar::detectEdge(const QPoint &pos) {
    QRect windowGeometry = trackedWindow->geometry();

    bool isNearLeft = pos.x() <= EDGE_THRESHOLD;
    bool isNearRight = pos.x() >= windowGeometry.width() - EDGE_THRESHOLD;
    bool isNearTop = pos.y() <= EDGE_THRESHOLD;
    bool isNearBottom = pos.y() >= windowGeometry.height() - EDGE_THRESHOLD;

    if (isNearTop && isNearLeft) {
        currentResizeRegion = TopLeftCorner;
    } else if (isNearTop && isNearRight) {
        currentResizeRegion = TopRightCorner;
    } else if (isNearBottom && isNearLeft) {
        currentResizeRegion = BottomLeftCorner;
    } else if (isNearBottom && isNearRight) {
        currentResizeRegion = BottomRightCorner;
    } else if (isNearTop) {
        currentResizeRegion = TopEdge;
    } else if (isNearBottom) {
        currentResizeRegion = BottomEdge;
    } else if (isNearLeft) {
        currentResizeRegion = LeftEdge;
    } else if (isNearRight) {
        currentResizeRegion = RightEdge;
    } else {
        currentResizeRegion = NoResize;
    }
}

QCursor TopBar::cursorForResizeRegion(ResizeRegion region) const {
    switch (region) {
        case TopEdge:
        case BottomEdge:
            return Qt::SizeVerCursor;
        case LeftEdge:
        case RightEdge:
            return Qt::SizeHorCursor;
        case TopLeftCorner:
        case BottomRightCorner:
            return Qt::SizeFDiagCursor;
        case TopRightCorner:
        case BottomLeftCorner:
            return Qt::SizeBDiagCursor;
        default:
            return Qt::ArrowCursor;
    }
}

void TopBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        isResizing = false;
        QApplication::restoreOverrideCursor();
    }
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
