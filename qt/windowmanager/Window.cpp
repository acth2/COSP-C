#include "Window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QDebug>

Window::Window(QWidget *parent) 
    : QMainWindow(parent), 
      xtermProcess(new QProcess(this)), 
      resizing(false), 
      dragging(false) {
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

    taskbarWidget = new QWidget(this);
    taskbarWidget->setFixedHeight(30);
    QHBoxLayout *taskbarLayout = new QHBoxLayout(taskbarWidget);

    closeButton = new QPushButton("✕", taskbarWidget);
    fullscreenButton = new QPushButton("❐", taskbarWidget);
    taskbarLayout->addWidget(fullscreenButton);
    taskbarLayout->addStretch();
    taskbarLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &Window::close);
    connect(fullscreenButton, &QPushButton::clicked, this, &Window::toggleFullScreen);

    xtermWidget = new QWidget(this);
    mainLayout->addWidget(taskbarWidget);
    mainLayout->addWidget(xtermWidget);
    setCentralWidget(centralWidget);
}

void Window::launchXTerm() {
    WId winId = xtermWidget->winId();
    QStringList arguments = {"-into", QString::number(winId), "-geometry", "80x24"};
    xtermProcess->start("xterm", arguments);
}

void Window::setXorgAppWindow(WId windowId) {
    attachTaskbar(windowId);
}

void Window::attachTaskbar(WId windowId) {
    qDebug() << "Attaching taskbar to Xorg window:" << windowId;
}

void Window::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
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
        int dx = event->globalX() - resizeStartPosition.x();
        int dy = event->globalY() - resizeStartPosition.y();
        resize(resizeStartSize.width() + dx, resizeStartSize.height() + dy);
    } else if (dragging) {
        move(event->globalPos() - dragStartPosition);
    }
    QMainWindow::mouseMoveEvent(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event) {
    dragging = false;
    resizing = false;
    QMainWindow::mouseReleaseEvent(event);
}

void Window::toggleFullScreen() {
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}
