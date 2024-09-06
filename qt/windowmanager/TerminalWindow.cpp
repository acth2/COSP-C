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
}

void TerminalWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F11) {
        if (isFullScreenMode) {
            showNormal();
            isFullScreenMode = false;
        } else {
            QScreen *screen = QApplication::primaryScreen();
            QRect screenGeometry = screen->geometry();
            setGeometry(screenGeometry);
            showFullScreen();
            isFullScreenMode = true;
        }
        updateTopBarVisibility();
    } else if (event->key() == Qt::Key_Escape && isFullScreenMode) {
        isFullScreenMode = false;
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry.width() / 2, screenGeometry.height() / 2, 800, 600);
    }
    
    QMainWindow::keyPressEvent(event);
}

void TerminalWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        // Check if resizing should start
        QRect rect = this->rect();
        QRect resizeHandle(0, 0, 10, 10);
        if (resizeHandle.adjusted(rect.width() - 10, rect.height() - 10, 0, 0).contains(event->pos())) {
            resizing = true;
            resizeStartSize = size();
            resizeStartPosition = event->globalPos();
        } else if (topBar->rect().contains(event->pos())) {
            dragging = true;
            dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        }
    }
    QMainWindow::mousePressEvent(event);
}

void TerminalWindow::mouseMoveEvent(QMouseEvent *event) {
    if (dragging) {
        move(event->globalPos() - dragStartPosition);
    } else if (resizing) {
        QSize newSize = resizeStartSize;
        newSize.setWidth(newSize.width() + (event->globalPos().x() - resizeStartPosition.x()));
        newSize.setHeight(newSize.height() + (event->globalPos().y() - resizeStartPosition.y()));

        // Ensure the new size is within reasonable bounds
        newSize.setWidth(qMax(newSize.width(), 100)); // Minimum width of 100 pixels
        newSize.setHeight(qMax(newSize.height(), 100)); // Minimum height of 100 pixels

        resize(newSize);
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
        setGeometry(screenGeometry);
        windowedFull = true;
    } else {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry.width() / 3, screenGeometry.height() / 3, 350, 350);
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
    fullscreenButton = new QPushButton("❐", topBar);

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
