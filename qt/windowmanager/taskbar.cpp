#include "taskbar.h"
#include "windowmanager.h"
#include <QApplication>
#include <QScreen>
#include <QDebug>

TaskBar::TaskBar(WindowManager *windowManager, QWidget *parent)
    : QWidget(parent), windowManager(windowManager), popupVisible(false) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setStyleSheet("background-color: #333333;");

    startButton = new QPushButton(this);
    startButton->setIcon(QIcon("/usr/cydra/cydra.png"));
    startButton->setIconSize(QSize(32, 32));
    startButton->setStyleSheet("border: none;");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(startButton, 0, Qt::AlignLeft | Qt::AlignBottom);
    layout->setContentsMargins(5, 5, 5, 5);
    setLayout(layout);

    connect(startButton, &QPushButton::clicked, this, &TaskBar::showPopup);

    adjustSizeToScreen();

    qApp->installEventFilter(this);
}

void TaskBar::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    adjustSizeToScreen();
}

void TaskBar::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    if (popupVisible && !popup->geometry().contains(event->globalPos())) {
        hidePopup();
    }
}

bool TaskBar::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (popupVisible && !popup->geometry().contains(mouseEvent->globalPos())) {
            hidePopup();
        }
    }
    return QWidget::eventFilter(watched, event);
}

void TaskBar::adjustSizeToScreen() {
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        setFixedSize(screenGeometry.width(), 40);
        move(0, screenGeometry.height() - height());
    }
}

void TaskBar::showPopup() {
    if (popupVisible) {
        hidePopup();
        return;
    }

    if (!popup) {
        popup = new QLabel(this);
        popup->setStyleSheet("background-color: #222222; color: #FFFFFF; border: 1px solid #444444;");
        popup->setFixedSize(500, 500);
        popup->setText("This is the popup content.");
        popup->setAlignment(Qt::AlignCenter);
    }

    QRect taskbarGeometry = geometry();
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    popup->move(0, taskbarGeometry.top() - popup->height());

    windowManager->appendLog("Taskbar opened.");
    popup->show();
    popupVisible = true;
    isPopupVisible = true;

    windowManager->appendLog("Popup shown.");
}

void TaskBar::hidePopup() {
    if (popup) {
        popup->hide();
        popupVisible = false;

        windowManager->appendLog("Taskbar hidden.");
    }
}
