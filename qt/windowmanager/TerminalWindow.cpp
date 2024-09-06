#include "TerminalWindow.h"
#include <QApplication>
#include <QScreen>
#include <QMouseEvent>
#include <QCursor>
#include <QVBoxLayout>
#include <QHBoxLayout>

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), isFullScreenMode(false), dragging(false), resizing(false) {
    setupUI();
}

void TerminalWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QRect windowRect = this->rect();
        int padding = 10;

        if (event->pos().x() >= windowRect.right() - padding && event->pos().y() >= windowRect.bottom() - padding) {
            resizing = true;
            resizeStartPosition = event->globalPos();
            resizeStartSize = this->size();
        } else if (topBar->rect().contains(event->pos())) {
            dragging = true;
            dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        }
    }
    QMainWindow::mousePressEvent(event);
}

void TerminalWindow::mouseMoveEvent(QMouseEvent *event) {
    int padding = 10;
    QRect windowRect = this->rect();

    if (event->pos().x() >= windowRect.right() - padding && event->pos().y() >= windowRect.bottom() - padding) {
        setCursor(Qt::SizeFDiagCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }

    if (resizing) {
        int newWidth = resizeStartSize.width() + (event->globalPos().x() - resizeStartPosition.x());
        int newHeight = resizeStartSize.height() + (event->globalPos().y() - resizeStartPosition.y());

        newWidth = std::max(newWidth, minimumWidth());
        newHeight = std::max(newHeight, minimumHeight());

        resize(newWidth, newHeight);
    } else if (dragging) {
        move(event->globalPos() - dragStartPosition);
    }

    QMainWindow::mouseMoveEvent(event);
}

void TerminalWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        resizing = false;
        dragging = false;
    }
    QMainWindow::mouseReleaseEvent(event);
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

void TerminalWindow::updateTopBarVisibility() {
    topBar->setVisible(!isFullScreenMode);
}
