#include "taskbar.h"
#include <QApplication>
#include <QScreen>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QKeyEvent>

TaskBar::TaskBar(QWidget *parent) : QWidget(parent) {
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

    popup = new QLabel(nullptr);
    popup->setFixedSize(500, 500);
    popup->setStyleSheet("background-color: #333333; border: 1px solid gray;"); 
    popup->hide();

    connect(startButton, &QPushButton::clicked, this, &TaskBar::showPopup);

    adjustSizeToScreen();
    installEventFilter();
}

void TaskBar::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    adjustSizeToScreen();
}

void TaskBar::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
}

void TaskBar::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape && popup->isVisible()) {
        closePopup();
    }
    QWidget::keyPressEvent(event);
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
    if (isPopupVisible) {
        closePopup();
    } else {
        popup->move(0, height() * 6);
        popup->show();
        isPopupVisible = true;
    }
}

void TaskBar::closePopup() {
    popup->hide();
    isPopupVisible = false;
}

void TaskBar::installEventFilter() {
    qApp->installEventFilter(this);
}

bool TaskBar::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (popup->isVisible() && !popup->geometry().contains(mouseEvent->globalPos())) {
            closePopup();
            return true;
        }
    }
    return QWidget::eventFilter(object, event);
}
