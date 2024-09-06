#include "TerminalWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QTextEdit>
#include <QPushButton>
#include <QDebug>
#include <QScreen>
#include <QCursor>

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), isFullScreenMode(false), dragging(false), isFullMode(false), windowedFull(false), resizing(false) {
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
    if (event->button() == Qt::LeftButton && topBar->rect().contains(event->pos())) {
        dragging = true;
        dragStartPosition = event->globalPos() - frameGeometry().topLeft();
    } else if (event->button() == Qt::LeftButton) {
        if (event->globalPos().x() >= (width() - 10) && event->globalPos().y() >= (height() - 10)) {
            resizing = true;
            resizeStartSize = size();
            resizeStartPosition = event->globalPos();
        }
    }
    QMainWindow::mousePressEvent(event);
}

void TerminalWindow::mouseMoveEvent(QMouseEvent *event) {
    if (dragging) {
        move(event->globalPos() - dragStartPosition);
    } else if (resizing) {
        QSize newSize = resizeStartSize + (event->globalPos() - resizeStartPosition).toSize();
        resize(newSize);
    } else if (event->globalPos().x() >= (width() - 10) && event->globalPos().y() >= (height() - 10)) {
        setCursor(Qt::SizeFDiagCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    QMainWindow::mouseMoveEvent(event);
}

void TerminalWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = false;
        resizing = false;
        setCursor(Qt::ArrowCursor);
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

void TerminalWindow::windowedFullScreen() {
    if (!windowedFull) {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry);
        windowedFull = true;
    } else {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry.width() / 2, screenGeometry.height() / 2, 350, 350);
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

void TerminalWindow::updateTopBarVisibility() {
    topBar->setVisible(!isFullScreenMode);
}
