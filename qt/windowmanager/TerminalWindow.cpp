#include "TerminalWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>

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

    topBar = new QWidget(this);
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(0, 0, 0, 0);

    closeButton = new QPushButton("X", topBar);
    fullscreenButton = new QPushButton("[ ]", topBar);

    topBarLayout->addWidget(fullscreenButton);
    topBarLayout->addStretch();
    topBarLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &TerminalWindow::close);
    connect(fullscreenButton, &QPushButton::clicked, this, &TerminalWindow::toggleFullScreen);

    mainLayout->addWidget(topBar);

    terminalWidget = new QTextEdit(this);
    terminalWidget->setText("This is a simulated terminal.");
    terminalWidget->setReadOnly(true);
    mainLayout->addWidget(terminalWidget);

    setCentralWidget(centralWidget);
    setWindowTitle("Window");
}
