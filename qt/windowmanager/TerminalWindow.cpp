#include "TerminalWindow.h"
#include <QApplication>
#include <QScreen>

TerminalWindow::TerminalWindow(QWidget *parent) 
    : QMainWindow(parent), terminalProcess(new QProcess(this)), isFullScreenMode(false) {

    setupUI();

    terminalProcess->start("xterm", QStringList() << "-into" << QString::number(centralWidget->winId()));
}

void TerminalWindow::setupUI() {
    centralWidget = new QWidget(this);
    layout = new QVBoxLayout(centralWidget);

    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QAction *fullscreenAction = new QAction("Toggle Full Screen", this);
    connect(fullscreenAction, &QAction::triggered, this, &TerminalWindow::toggleFullScreen);
    menuBar->addAction(fullscreenAction);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
    
    resize(500, 500);
    centerWindow();
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
    } else {
        normalGeometry = geometry();
        showFullScreen();
        isFullScreenMode = true;
        menuBar()->hide();
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
