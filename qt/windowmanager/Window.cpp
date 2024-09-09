#include "Window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QDebug>

Window::Window(QWidget *parent) : QMainWindow(parent), xtermProcess(new QProcess(this)) {
    setupUI();
    launchXTerm();
}

Window::~Window() {
    if (xtermProcess->state() == QProcess::Running) {
        xtermProcess->terminate();
        xtermProcess->waitForFinished();
    }
}

void Window::setupUI() {
    centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QWidget *topBar = new QWidget(this);
    topBar->setFixedHeight(30);
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    
    closeButton = new QPushButton("✕", topBar);
    fullscreenButton = new QPushButton("❐", topBar);
    topBarLayout->addWidget(fullscreenButton);
    topBarLayout->addStretch();
    topBarLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &Window::close);
    connect(fullscreenButton, &QPushButton::clicked, this, &Window::toggleFullScreen);

    xtermWidget = new QWidget(this);
    mainLayout->addWidget(topBar);
    mainLayout->addWidget(xtermWidget);
    setCentralWidget(centralWidget);
}

void Window::launchXTerm() {
    WId winId = xtermWidget->winId();
    QStringList arguments = {"-into", QString::number(winId), "-geometry", "80x24"};
    xtermProcess->start("xterm", arguments);
}

void Window::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    // Handling terminal resizing
    if (xtermProcess->state() == QProcess::Running) {
        int newColumns = event->size().width() / 9;
        int newRows = event->size().height() / 18;
        QString resizeCommand = QString("printf '\\e[8;%1;%2t'").arg(newRows).arg(newColumns);
        xtermProcess->write(resizeCommand.toUtf8());
    }
}

void Window::mousePressEvent(QMouseEvent *event) {
    int margin = 10;
    bool onRightEdge = event->x() > (width() - margin);
    bool onBottomEdge = event->y() > (height() - margin);

    if (onRightEdge || onBottomEdge) {
        resizing = true;
        resizeStartPosition = event->globalPos();
        resizeStartSize = size();
    } else {
        dragging = true;
        dragStartPosition = event->globalPos() - frameGeometry().topLeft();
    }
    QMainWindow::mousePressEvent(event);
}

void Window::mouseMoveEvent(QMouseEvent *event) {
    if (resizing) {
        QSize newSize = resizeStartSize + QSize(event->globalPos().x() - resizeStartPosition.x(),
                                                event->globalPos().y() - resizeStartPosition.y());
        resize(newSize);
    } else if (dragging) {
        move(event->globalPos() - dragStartPosition);
    }
    QMainWindow::mouseMoveEvent(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event) {
    resizing = false;
    dragging = false;
    QMainWindow::mouseReleaseEvent(event);
}

void Window::toggleFullScreen() {
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}
