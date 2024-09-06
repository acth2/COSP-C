#include "TerminalWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QTextEdit>
#include <QPushButton>
#include <QDebug>
#include <QScreen>

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), isFullScreenMode(false), dragging(false), resizing(false) {
    setupUI();
    setWindowFlags(windowFlags() | Qt::WindowResizeButtonHint);
}

void TerminalWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F11) {
        if (isFullScreenMode) {
            showNormal();
            isFullScreenMode = false;
    } else {
            QScreen *screen = QApplication::primaryScreen();
            QRect screenGeometry = screen->geometry();

            int screenWidth = screenGeometry.width();
            int screenHeight = screenGeometry.height();

            setGeometry(0, 0, screenWidth, screenHeight);
            showFullScreen();       
        
            isFullScreenMode = true;
    }
    updateTopBarVisibility();
    } else if (event->key() == Qt::Key_Escape && isFullScreenMode) {
        isFullScreenMode = false;
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();

        int screenWidth = screenGeometry.width();
        int screenHeight = screenGeometry.height();

        setGeometry(screenGeometry.width() / 2, screenGeometry.height() / 2, 800, 600);
    }
    
    QMainWindow::keyPressEvent(event);
}

void TerminalWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (topBar->rect().contains(event->pos())) {
            dragging = true;
            dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        } else if (event->pos().x() > width() - 10 && event->pos().y() > height() - 10) {
            resizing = true;
            resizeStartPosition = event->globalPos();
            resizeStartSize = size();
        }
    }
    QMainWindow::mousePressEvent(event);
}

void TerminalWindow::mouseMoveEvent(QMouseEvent *event) {
    if (dragging) {
        move(event->globalPos() - dragStartPosition);
    } else if (resizing) {
        QSize newSize = resizeStartSize + (event->globalPos() - resizeStartPosition);
        resize(newSize.width(), newSize.height());
    }
    QMainWindow::mouseMoveEvent(event);
}

void TerminalWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = false;
        resizing = false;
    }
    QMainWindow::mouseReleaseEvent(event);
}

void TerminalWindow::toggleFullScreen() {
    if (isFullMode) {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();

        int screenWidth = screenGeometry.width();
        int screenHeight = screenGeometry.height();

        setGeometry(screenGeometry.width() / 2, screenGeometry.height() / 2, 800, 600);
        isFullMode = false;
    } else {
        setGeometry(0, 0, 800, 500);
        isFullMode = true;
    }
    updateTopBarVisibility();
}

void TerminalWindow::updateTopBarVisibility() {
    topBar->setVisible(!isFullScreenMode);
}

void TerminalWindow::windowedFullScreen() {
    if (!windowedFull) {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();

        int screenWidth = screenGeometry.width();
        int screenHeight = screenGeometry.height();

        setGeometry(0, 0, screenWidth, screenHeight);
        windowedFull = true;
    } else {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();

        int screenWidth = screenGeometry.width();
        int screenHeight = screenGeometry.height();

        setGeometry(screenWidth / 2, screenHeight / 2, 350, 350);
        windowedFull = false;
    }
}

void TerminalWindow::setupUI() {
    centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    topBar = new QWidget(this);
    topBar->setFixedHeight(30);
    topBar->setStyleSheet("background-color: #333; color: white;");
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(0, 0, 0, 0);

    closeButton = new QPushButton("✕", topBar);
    fullscreenButton = new QPushButton("⛶", topBar);

    topBarLayout->addWidget(fullscreenButton);
    topBarLayout->addStretch();
    topBarLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &TerminalWindow::close);
    connect(fullscreenButton, &QPushButton::clicked, this, &TerminalWindow::windowedFullScreen);

    terminalWidget = new QTextEdit(this);
    terminalWidget->setText("This is a simulated terminal.");
    terminalWidget->setReadOnly(true);

    mainLayout->addWidget(topBar);
    mainLayout->addWidget(terminalWidget);

    setCentralWidget(centralWidget);
    setWindowTitle("Terminal Window");

    updateTopBarVisibility();
}
