#include "TerminalWindow.h"
#include <QApplication>
#include <QScreen>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>

TerminalWindow::TerminalWindow(QWidget *parent) 
    : QMainWindow(parent), terminalProcess(new QProcess(this)), isFullScreenMode(false) {

    setupUI();

    terminalProcess->start("xterm", QStringList() << "-into" << QString::number(centralWidget->winId()));
}

void TerminalWindow::setupUI() {
    centralWidget = new QWidget(this);
    layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);

    setupCustomTitleBar();
    
    resize(500, 500);
    centerWindow();
}

void TerminalWindow::setupCustomTitleBar() {
    customTitleBar = new QWidget(this);
    customTitleBar->setStyleSheet("background-color: #2c3e50; padding: 5px;");

    QHBoxLayout *titleBarLayout = new QHBoxLayout(customTitleBar);
    titleBarLayout->setContentsMargins(5, 0, 5, 0);

    fullscreenButton = new QPushButton("⛶", this);
    fullscreenButton->setFixedSize(30, 30);
    connect(fullscreenButton, &QPushButton::clicked, this, &TerminalWindow::toggleFullScreen);
    titleBarLayout->addWidget(fullscreenButton);

    QLabel *titleLabel = new QLabel("Terminal", this);
    titleLabel->setStyleSheet("color: white; font-size: 14px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleBarLayout->addWidget(titleLabel);

    closeButton = new QPushButton("✕", this);
    closeButton->setFixedSize(30, 30);
    connect(closeButton, &QPushButton::clicked, this, &TerminalWindow::close);
    titleBarLayout->addWidget(closeButton);

    layout->addWidget(customTitleBar);
}

void TerminalWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F11) {
        toggleFullScreen();
    } else if (event->key() == Qt::Key_Escape && isFullScreenMode) {
        toggleFullScreen();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void TerminalWindow::toggleFullScreen() {
    if (isFullScreenMode) {
        setGeometry(normalGeometry);
        isFullScreenMode = false;
        menuBar()->show();
        customTitleBar->show();
    } else {
        normalGeometry = geometry();
        showFullScreen();
        isFullScreenMode = true;
        customTitleBar->hide();
    }
}

void TerminalWindow::centerWindow() {
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
}
