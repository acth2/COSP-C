#include "TerminalWindow.h"
#include <QApplication>
#include <QScreen>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QProcess>
#include <QMenuBar>

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), terminalProcess(new QProcess(this)), isFullScreenMode(false) {

    setupUI();

    terminalProcess->start("xterm", QStringList() << "-into" << QString::number(terminalWidget->winId()));
}

void TerminalWindow::setupUI() {
    centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *topBar = new QWidget(this);
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(5, 5, 5, 5);

    QPushButton *closeButton = new QPushButton("X", this);
    QPushButton *fullscreenButton = new QPushButton("[ ]", this);

    topBarLayout->addWidget(fullscreenButton);
    topBarLayout->addStretch();
    topBarLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &TerminalWindow::close);
    connect(fullscreenButton, &QPushButton::clicked, this, &TerminalWindow::toggleFullScreen);

    mainLayout->addWidget(topBar);

    terminalWidget = new QWidget(this);
    terminalWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mainLayout->addWidget(terminalWidget);

    setCentralWidget(centralWidget);
}

void TerminalWindow::toggleFullScreen() {
    if (isFullScreenMode) {
        showNormal();
        isFullScreenMode = false;
    } else {
        showFullScreen();
        isFullScreenMode = true;
    }
}

void TerminalWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F11) {
        toggleFullScreen();
    } else if (event->key() == Qt::Key_Escape && isFullScreenMode) {
        toggleFullScreen();
    }
    QMainWindow::keyPressEvent(event);
}
