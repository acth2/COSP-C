#include "userinteractright.h"
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QApplication>
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QCursor>
#include <QScreen>
#include <QWindow>

UserInteractRight::UserInteractRight(QWidget *parent) 
    : QWidget(parent), isDarkMode(false), resizeMode(false) {

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setupUI();

    setFixedSize(200, 250);

    setWindowTitle("A2WM");

    if (QFile::exists("/usr/cydra/settings/darkmode")) {
        isDarkMode = true;
    }
    
    applyStyles();

    qApp->installEventFilter(this);
}

void UserInteractRight::setupUI() {
    button1 = new QPushButton("Terminal", this);
    button2 = new QPushButton("Resize Mode", this);
    button3 = new QPushButton("Thing 3 bruh", this);
    textLabel = new QLabel("Control panel", this);

    textLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(textLabel);
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->setSpacing(10); 
    layout->setContentsMargins(15, 15, 15, 15);
    setLayout(layout);

    connect(button1, &QPushButton::clicked, this, &UserInteractRight::button1Clicked);
    connect(button2, &QPushButton::clicked, this, &UserInteractRight::button2Clicked);
}

void UserInteractRight::applyStyles() {
    QString buttonStyle;
    QString labelStyle;

    if (isDarkMode) {
        buttonStyle = R"(
            QPushButton {
                background-color: #cfcfcf; 
                color: #595853;
                border: none;
                border-radius: 5px; 
                padding: 8px 16px;
                font-size: 12px;
                font-weight: bold;
                min-width: 100px; width 
            }
            QPushButton:hover {
                color: #bab9b5;
                background-color: #52514e 
            }
        )";

        labelStyle = R"(
            QLabel {
                color: #bab9b5;
                font-size: 14px;
                font-weight: medium;
                margin-bottom: 10px;
                background-color: transparent;
            }
        )";
    } else {
        buttonStyle = R"(
            QPushButton {
                background-color: #0078D4;
                color: white;
                border: none;
                border-radius: 5px;
                padding: 8px 16px;
                font-size: 12px;
                font-weight: bold;
                min-width: 100px; 
            }
            QPushButton:hover {
                background-color: #005A9E;
            }
        )";

        labelStyle = R"(
            QLabel {
                color: #333333;
                font-size: 14px;
                font-weight: medium;
                margin-bottom: 10px;
                background-color: transparent;
            }
        )";
    }

    textLabel->setStyleSheet(labelStyle);
    button1->setStyleSheet(buttonStyle);
    button2->setStyleSheet(buttonStyle);
    button3->setStyleSheet(buttonStyle);
}

void UserInteractRight::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        QPoint cursorPos = event->globalPos();
        move(cursorPos.x() - width() / 2, cursorPos.y() - height() / 2);
        show();
    }
    
    QWidget::mousePressEvent(event);
}

void UserInteractRight::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        closeIfClickedOutside(event);
    }
    QWidget::mouseReleaseEvent(event);
}

void UserInteractRight::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.setPen(QPen(Qt::black, 2)); 
    if (isDarkMode) {
        painter.setBrush(QColor(31, 30, 29, 240)); 
    } else {
        painter.setBrush(QColor(255, 255, 255, 240)); 
    }
    painter.drawRect(rect());

    QWidget::paintEvent(event);
}

void UserInteractRight::button1Clicked() {
    QProcess *xtermProcess = new QProcess(this);

    QString program = "qterminal";
    QStringList arguments;

    xtermProcess->start(program, arguments);

    connect(xtermProcess, &QProcess::errorOccurred, [](QProcess::ProcessError error) {
        qDebug() << "Error occurred:" << error;
    });

    connect(xtermProcess, &QProcess::started, []() {
        qDebug() << "qterminal started successfully";
    });
    close();
}

void UserInteractRight::button2Clicked() {
    waitingForClick = true;
    QApplication::setOverrideCursor(Qt::SizeAllCursor);

    qDebug() << "Resize mode enabled. Click on a window to resize it.";
}

void UserInteractRight::onWindowClick(QWindow *window) {
    if (waitingForClick && window) {
        QWidget *containerWidget = window->findChild<QWidget*>();
        if (!containerWidget) {
            qDebug() << "Failed to find container widget for window:" << window->title();
            return;
        }

        qDebug() << "Container widget clicked for resizing.";
        currentResizingWidget = containerWidget;
        initialClickPos = QCursor::pos();

        resizeMode = true;
        waitingForClick = false;
    }
}

void UserInteractRight::onMouseMove(QMouseEvent *event) {
    if (resizeMode && currentResizingWidget) {
        QPoint currentPos = QCursor::pos();
        int deltaX = currentPos.x() - initialClickPos.x();
        int deltaY = currentPos.y() - initialClickPos.y();

        QWindow* window = currentResizingWidget->windowHandle();
        if (!window) {
            qDebug() << "Failed to get QWindow for resizing";
            return;
        }

        WId windowId = window->winId();

        int newWidth = currentResizingWidget->width() + deltaX;
        int newHeight = currentResizingWidget->height() + deltaY;
        windowManagerInstance->resizeTrackedWindow(windowId, newWidth, newHeight);

        initialClickPos = currentPos;
    }
}

void UserInteractRight::onMouseRelease(QMouseEvent *event) {
    if (resizeMode) {
        resizeMode = false;
        QApplication::restoreOverrideCursor();
        currentResizingWidget = nullptr;
        qDebug() << "Resize mode disabled";
    }
}

bool UserInteractRight::eventFilter(QObject *obj, QEvent *event) {
    if (waitingForClick) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                QPoint cursorPos = mouseEvent->globalPos();
                QWindow *clickedWindow = QGuiApplication::topLevelAt(cursorPos);
                if (clickedWindow) {
                    qDebug() << "Detected click on window:" << clickedWindow->title();
                    onWindowClick(clickedWindow);
                    return true;
                }
            }
        }
    } 
    else if (resizeMode) {
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            onMouseMove(mouseEvent);
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            onMouseRelease(mouseEvent);
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}

void UserInteractRight::closeIfClickedOutside(QMouseEvent *event) {
    if (!rect().contains(mapFromGlobal(event->globalPos()))) {
        close();
    }
}
