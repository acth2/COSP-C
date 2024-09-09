#include "Window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>

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

    // Custom top bar
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
    if (xtermProcess->state() == QProcess::Running) {
        int newColumns = event->size().width() / 9;
        int newRows = event->size().height() / 18;
        QString resizeCommand = QString("printf '\\e[8;%1;%2t'").arg(newRows).arg(newColumns);
        xtermProcess->write(resizeCommand.toUtf8());
    }
}

void Window::toggleFullScreen() {
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}
