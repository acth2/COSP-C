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
        handle->setGeometry(0, 0, 10, 100);
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

        leftHandle->setGeometry(windowGeometry.x() - handleWidth, windowGeometry.y(), handleWidth, windowGeometry.height());
        rightHandle->setGeometry(windowGeometry.x() + windowGeometry.width(), windowGeometry.y(), handleWidth, windowGeometry.height());
        bottomHandle->setGeometry(windowGeometry.x(), windowGeometry.y() + windowGeometry.height(), windowGeometry.width(), handleWidth);
        
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
        QPoint mousePos = event->globalPos();

        if (leftHandle->geometry().contains(mousePos)) {
            isResizingLeft = true;
            resizeStartPos = event->globalPos();
            windowStartSize = trackedWindow->geometry().size();
        } else if (rightHandle->geometry().contains(mousePos)) {
            isResizingRight = true;
            resizeStartPos = event->globalPos();
            windowStartSize = trackedWindow->geometry().size();
        } else if (bottomHandle->geometry().contains(mousePos)) {
            isResizingBottom = true;
            resizeStartPos = event->globalPos();
            windowStartSize = trackedWindow->geometry().size();
        } else {
            isDragging = true;
            dragStartPos = event->globalPos();
            windowStartPos = trackedWindow->geometry().topLeft();
        }
    }
    updatePosition();
}

void TopBar::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging && trackedWindow) {
    } else if (isResizingLeft && trackedWindow) {
        int newWidth = windowStartSize.width() - (event->globalPos().x() - resizeStartPos.x());
        trackedWindow->setGeometry(trackedWindow->geometry().x() + (event->globalPos().x() - resizeStartPos.x()), trackedWindow->geometry().y(), newWidth, trackedWindow->geometry().height());
    } else if (isResizingRight && trackedWindow) {
        int newWidth = windowStartSize.width() + (event->globalPos().x() - resizeStartPos.x());
        trackedWindow->setGeometry(trackedWindow->geometry().x(), trackedWindow->geometry().y(), newWidth, trackedWindow->geometry().height());
    } else if (isResizingBottom && trackedWindow) {
        int newHeight = windowStartSize.height() + (event->globalPos().y() - resizeStartPos.y());
        trackedWindow->setGeometry(trackedWindow->geometry().x(), trackedWindow->geometry().y(), trackedWindow->geometry().width(), newHeight);
    }
    updatePosition();
}

void TopBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        isResizingLeft = false;
        isResizingRight = false;
        isResizingBottom = false;
    }
    updatePosition();
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
