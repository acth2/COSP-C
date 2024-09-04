#include "taskbar.h"
#include <QApplication>
#include <QScreen>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

TaskBar::TaskBar(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setStyleSheet("background-color: #333333;");

    startButton = new QPushButton(this);
    startButton->setIcon(QIcon("/usr/cydra/cydra.png"));
    startButton->setIconSize(QSize(32, 32));
    startButton->setStyleSheet("border: none;");

    powerButton = new QPushButton(this);
    powerButton->setIcon(QIcon("/usr/cydra/icons/power.png"));
    powerButton->setIconSize(QSize(32, 32));
    powerButton->setStyleSheet("border: none;");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(startButton, 0, Qt::AlignLeft | Qt::AlignBottom);
    layout->addWidget(powerButton, 0, Qt::AlignRight | Qt::AlignBottom);
    layout->setContentsMargins(5, 5, 5, 5);
    setLayout(layout);

    popup = new QLabel(nullptr);
    popup->setFixedSize(500, 500);
    popup->setStyleSheet("background-color: #333333; border: 1px solid gray;");
    popup->hide();

    connect(startButton, &QPushButton::clicked, this, &TaskBar::showPopup);
    connect(powerButton, &QPushButton::clicked, this, &TaskBar::showPowerMenu);

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
        popup->move(0, height() * 5.7);
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

void TaskBar::showPowerMenu() {
    QWidget *overlay = new QWidget(this);
    overlay->setStyleSheet("background: rgba(0, 0, 0, 0.7);");
    overlay->setGeometry(QApplication::primaryScreen()->geometry());
    overlay->show();

    QDialog *powerDialog = new QDialog(this);
    powerDialog->setWindowTitle("Power Options");
    powerDialog->setModal(true);
    QVBoxLayout *dialogLayout = new QVBoxLayout(powerDialog);

    QLabel *infoLabel = new QLabel("Do you want to reboot or power off?", powerDialog);
    QPushButton *rebootButton = new QPushButton("Reboot", powerDialog);
    QPushButton *powerOffButton = new QPushButton("Power Off", powerDialog);

    connect(rebootButton, &QPushButton::clicked, [=]() {
        qApp->exit(1);
    });

    connect(powerOffButton, &QPushButton::clicked, [=]() {
        qApp->exit(0);
    });

    dialogLayout->addWidget(infoLabel);
    dialogLayout->addWidget(rebootButton);
    dialogLayout->addWidget(powerOffButton);

    powerDialog->exec();

    overlay->deleteLater();
    powerDialog->deleteLater();
}
