#include "TerminalWindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

TerminalWindow::TerminalWindow(QWidget *parent) 
    : QMainWindow(parent), isFullScreenMode(false) {
    setupUI();
}

void TerminalWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F11) {
        toggleFullScreen();
    } else if (event->key() == Qt::Key_Escape && isFullScreenMode) {
        toggleFullScreen();
    }
    QMainWindow::keyPressEvent(event);
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

    QLabel *label = new QLabel("Regular window", this);
    mainLayout->addWidget(label);

    setCentralWidget(centralWidget);
}
