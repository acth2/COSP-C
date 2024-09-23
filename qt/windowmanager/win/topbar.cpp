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
    : QWidget(parent), trackedWindow(parentWindow), isDragging(false),
      isResizingLeft(false), isResizingRight(false), isResizingBottom(false) {
    setMouseTracking(true);
        
    if (QFile::exists("/usr/cydra/settings/darkmode")) {
        isDarkMode = true;
    } else {
        isDarkMode = false;
    }

    leftHandle = new QWindow();
    rightHandle = new QWindow();
    bottomHandle = new QWindow();

    for (QWindow* handle : {leftHandle, rightHandle, bottomHandle}) {
        handle->setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        handle->setGeometry(0, 0, 20, 20);
        handle->show();
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

        int handleWidth = 10;

        leftHandle->setGeometry(0, 0, 50, 50);
        rightHandle->setGeometry(50, 0, 50, 50);
        bottomHandle->setGeometry(0, 50, 50, 50);

        leftHandle->show();
        rightHandle->show();
        bottomHandle->show();
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
void TopBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QRect windowGeometry = trackedWindow->geometry();
        QPoint localMousePos = event->pos();

        resizeDirection = getResizeDirection(localMousePos);

        if (resizeDirection != None) {
            isResizing = true;
            resizeStartPos = event->globalPos();
        } else {
            isDragging = true;
            dragStartPos = event->globalPos();
        }
    }
}

void TopBar::mouseMoveEvent(QMouseEvent *event) {
    QPoint localMousePos = event->pos();
    updateCursorShape(localMousePos);

    if (isDragging && !isResizing) {
        QPoint delta = event->globalPos() - dragStartPos;
        trackedWindow->setPosition(trackedWindow->position() + delta);
        dragStartPos = event->globalPos();
    }

    if (isResizing && !isDragging) {
        QPoint currentMousePos = event->globalPos();
        QRect windowGeometry = trackedWindow->geometry();
        int deltaX = currentMousePos.x() - resizeStartPos.x();
        int deltaY = currentMousePos.y() - resizeStartPos.y();

        switch (resizeDirection) {
            case Left:
                trackedWindow->setGeometry(windowGeometry.x() + deltaX, windowGeometry.y(),
                                           windowGeometry.width() - deltaX, windowGeometry.height());
                break;
            case Right:
                trackedWindow->resize(windowGeometry.width() + deltaX, windowGeometry.height());
                break;
            case Bottom:
                trackedWindow->resize(windowGeometry.width(), windowGeometry.height() + deltaY);
                break;
            case BottomLeft:
                trackedWindow->setGeometry(windowGeometry.x() + deltaX, windowGeometry.y(),
                                           windowGeometry.width() - deltaX, windowGeometry.height() + deltaY);
                break;
            case BottomRight:
                trackedWindow->resize(windowGeometry.width() + deltaX, windowGeometry.height() + deltaY);
                break;
            default:
                break;
        }

        resizeStartPos = currentMousePos;
    }
}

void TopBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        isResizing = false;
        resizeDirection = None;
        QApplication::restoreOverrideCursor();
    }
}

void TopBar::updateCursorShape(const QPoint &localMousePos) {
    ResizeDirection direction = getResizeDirection(localMousePos);

    switch (direction) {
        case Left:
        case Right:
            QApplication::setOverrideCursor(Qt::SizeHorCursor);
            break;
        case Bottom:
        case BottomLeft:
        case BottomRight:
            QApplication::setOverrideCursor(Qt::SizeVerCursor);
            break;
        default:
            QApplication::restoreOverrideCursor();
            break;
    }
}

TopBar::ResizeDirection TopBar::getResizeDirection(const QPoint &localMousePos) {
    const int margin = 10;
    QRect windowGeometry = trackedWindow->geometry();

    bool nearLeft = localMousePos.x() <= margin;
    bool nearRight = localMousePos.x() >= windowGeometry.width() - margin;
    bool nearBottom = localMousePos.y() >= windowGeometry.height() - margin;

    if (nearLeft && nearBottom) {
        return BottomLeft;
    } else if (nearRight && nearBottom) {
        return BottomRight;
    } else if (nearLeft) {
        return Left;
    } else if (nearRight) {
        return Right;
    } else if (nearBottom) {
        return Bottom;
    }

    return None;
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
