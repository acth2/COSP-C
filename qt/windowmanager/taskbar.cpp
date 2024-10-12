#include "taskbar.h"
#include <QtCore/qtextstream.h>
#include <QApplication>
#include <QScreen>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QMessageBox>
#include <QWidget>
#include <QFile>
#include <QTimer>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

TaskBar::TaskBar(QWidget *parent) : QWidget(parent) {
    if (QFile::exists("/usr/cydra/settings/darkmode")) {
        isDarkMode = true;
    } else {
        setStyleSheet("background-color: #cfcfcf; border: 1px solid #DDDDDD;");
    }
    
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    if (isDarkMode) {
        setStyleSheet("background-color: #333333;");
    }

    startButton = new QPushButton(this);
    if (!isDarkMode) {
        startButton->setIcon(QIcon("/usr/cydra/cydradm.png"));
    } else {
        startButton->setIcon(QIcon("/usr/cydra/cydra.png"));
    }
    startButton->setIconSize(QSize(32, 32));
    startButton->setStyleSheet("border: none;");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(startButton, 0, Qt::AlignLeft | Qt::AlignBottom);
    layout->setContentsMargins(5, 5, 5, 5);

    setLayout(layout);

    popup = new QLabel(nullptr);
    popup->setFixedSize(500, 500);
    if (isDarkMode) {
        popup->setStyleSheet("background-color: #333333; border: 1px solid #000000;");
    } else {
        popup->setStyleSheet("background-color: #fff; border: 1px solid #000000;"); 
    }
    
    popup->hide();

    powerButton = new QPushButton(popup);
    powerButton->setIcon(QIcon("/usr/cydra/icons/power.png"));
    powerButton->setIconSize(QSize(32, 32));
    powerButton->setStyleSheet("border: none;");

    QVBoxLayout *popupLayout = new QVBoxLayout(popup);
    popupLayout->addWidget(powerButton, 0, Qt::AlignBottom | Qt::AlignLeft);
    popupLayout->setContentsMargins(10, 10, 10, 10);

    connect(powerButton, &QPushButton::clicked, this, &TaskBar::showPowerMenu);
    connect(startButton, &QPushButton::clicked, this, &TaskBar::showPopup);

    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    adjustSizeToScreen();
    installEventFilter();

    onLoopTimer = new QTimer(this);
    connect(onLoopTimer, &QTimer::timeout, this, &TaskBar::onLoop);
    onLoopTimer->start(1500);
}

void TaskBar::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    adjustSizeToScreen();
}

Display *x2Display;
void TaskBar::onLoop() {
    if (x2Display) {
        Atom netWmWindowType = XInternAtom(x2Display, "_NET_WM_WINDOW_TYPE", False);
        Atom netWmWindowTypeNormal = XInternAtom(x2Display, "_NET_WM_WINDOW_TYPE_NORMAL", False);
        Atom netWmWindowTypeDesktop = XInternAtom(x2Display, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
        Atom netWmWindowTypeDock = XInternAtom(x2Display, "_NET_WM_WINDOW_TYPE_DOCK", False);
        Atom netWmWindowTypeToolbar = XInternAtom(x2Display, "_NET_WM_WINDOW_TYPE_TOOLBAR", False);
        Atom netWmWindowTypeMenu = XInternAtom(x2Display, "_NET_WM_WINDOW_TYPE_MENU", False);
        Atom netWmWindowTypeUtility = XInternAtom(x2Display, "_NET_WM_WINDOW_TYPE_UTILITY", False);
        Atom netWmWindowTypeSplash = XInternAtom(x2Display, "_NET_WM_WINDOW_TYPE_SPLASH", False);
        Atom netWmWindowTypeDialog = XInternAtom(x2Display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
        
        Window windowRoot = DefaultRootWindow(x2Display);
        Window parent, *children;
        unsigned int nChildren;

        if (XQueryTree(x2Display, windowRoot, &windowRoot, &parent, &children, &nChildren)) {
            for (unsigned int i = 0; i < nChildren; i++) {
                Window child = children[i];

                char *windowName = nullptr;
                if (XFetchName(x2Display, child, &windowName) && windowName) {
                    QString name(windowName);
                    XFree(windowName);
                }

                Atom type;
                int format;
                unsigned long nItems, bytesAfter;
                unsigned char *data = nullptr;

                if (XGetWindowProperty(x2Display, child, netWmWindowType, 0, 1, False, XA_ATOM,
                                   &type, &format, &nItems, &bytesAfter, &data) == Success) {
                    if (data) {
                        Atom *atoms = (Atom *)data;
                        if (atoms[0] != netWmWindowTypeDock &&
                            atoms[0] != netWmWindowTypeToolbar &&
                            atoms[0] != netWmWindowTypeMenu &&
                            atoms[0] != netWmWindowTypeUtility &&
                            atoms[0] != netWmWindowTypeSplash &&
                            atoms[0] != netWmWindowTypeDialog) {
                            XFree(data);
                            continue;
                        }
                    }
                }

                XWindowAttributes attributes;
                if (XGetWindowAttributes(x2Display, child, &attributes) == 0 || attributes.map_state != IsViewable) {
                    continue;
                }

                QRect windowGeometry(attributes.x, attributes.y, attributes.width, attributes.height);

                if (windowGeometry.width() == 0 || windowGeometry.height() == 0) {
                    continue;
                }

                    char *windowName2 = nullptr;
                    if (XFetchName(x2Display, child, &windowName2) && windowName2) {
                        QString name2(windowName2);
                    }
                        XFree(children);
                }
        }
    }
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
        popup->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        isPopupVisible = true;
    }
}

void TaskBar::closePopup() {
    popup->hide();
    isPopupVisible = false;
}

void TaskBar::showPowerMenu() {
    if (powerMenuVisible) {
        closePowerMenu();
    } else {
        QWidget *overlay = new QWidget(nullptr);
        overlay->setStyleSheet("background: rgba(0, 0, 0, 0.7);");
        overlay->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
        overlay->setGeometry(QApplication::primaryScreen()->geometry());
        overlay->show();

        QDialog *powerDialog = new QDialog();
        powerDialog->setWindowTitle("Power Options");
        powerDialog->setModal(true);
        powerDialog->setAttribute(Qt::WA_DeleteOnClose);
        if (isDarkMode) {
        powerDialog->setStyleSheet(R"(
                QDialog {
                    background-color: #2e2e2d;
                    border-radius: 10px;
                }
            )");
        }

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

        QVBoxLayout *dialogLayout = new QVBoxLayout(powerDialog);
        QLabel *infoLabel = new QLabel("Press ESC to go back", powerDialog);
        infoLabel->setStyleSheet(labelStyle);

        QPushButton *rebootButton = new QPushButton("Reboot", powerDialog);
        rebootButton->setStyleSheet(buttonStyle);

        QPushButton *powerOffButton = new QPushButton("Power Off", powerDialog);
        powerOffButton->setStyleSheet(buttonStyle);

        connect(rebootButton, &QPushButton::clicked, [=]() {
            qApp->exit(1);
        });

        connect(powerOffButton, &QPushButton::clicked, [=]() {
            qApp->exit(0);
        });

        dialogLayout->addWidget(infoLabel);
        dialogLayout->addWidget(rebootButton);
        dialogLayout->addWidget(powerOffButton);

        powerDialog->adjustSize();

        QRect screenGeometry = QApplication::primaryScreen()->geometry();
        int x = (screenGeometry.width() - powerDialog->width()) / 2;
        int y = (screenGeometry.height() - powerDialog->height()) / 2;
        powerDialog->move(x, y);

        connect(powerDialog, &QDialog::finished, [=]() {
            overlay->close();
            overlay->deleteLater();
            powerMenuVisible = false;
        });

        powerDialog->exec();

        powerMenuVisible = true;
    }
}

void TaskBar::closePowerMenu() {
    if (powerMenuVisible) {
        powerMenuVisible = false;
    }
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
