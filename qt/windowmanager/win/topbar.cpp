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

    leftHandle = new QWidget();
    rightHandle = new QWidget();
    bottomHandle = new QWidget();


    for (QWidget* handle : {leftHandle, rightHandle, bottomHandle}) {
        handle->setAttribute(Qt::WA_TransparentForMouseEvents);
        handle->setAttribute(Qt::WA_NoSystemBackground);
        handle->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        handle->setStyleSheet("background-color: rgba(255, 255, 255, 150);");
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
        int handleHeight = windowGeometry.height();

        leftHandle->setGeometry(windowGeometry.x() - handleWidth, windowGeometry.y(), handleWidth, handleHeight);
        rightHandle->setGeometry(windowGeometry.x() + windowGeometry.width(), windowGeometry.y(), handleWidth, handleHeight);
        bottomHandle->setGeometry(windowGeometry.x(), windowGeometry.y() + windowGeometry.height(), windowGeometry.width(), handleWidth);
        
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
        isDragging = true;
        dragStartPos = event->globalPos();
        windowStartPos = trackedWindow->geometry().topLeft();
    }
}

void TopBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
}

void TopBar::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging && trackedWindow) {
        QPoint currentPos = event->globalPos();
        QPoint delta = currentPos - dragStartPos;
        QRect windowGeometry = trackedWindow->geometry();
        windowGeometry.moveTopLeft(windowStartPos + delta);
        trackedWindow->setGeometry(windowGeometry);

        updatePosition();
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
