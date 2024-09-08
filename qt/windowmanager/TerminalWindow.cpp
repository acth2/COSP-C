#include "TerminalWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QProcess>
#include <QScreen>
#include <QCursor>
#include <QKeyEvent>
#include <QWidget>
#include <QResizeEvent>
#include <QTimer>
#include <QDebug>

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), xtermProcess(new QProcess(this)), isFullScreenMode(false) {
    setupUI();
    setCursor(Qt::ArrowCursor);
    
    QTimer::singleShot(100, this, &TerminalWindow::launchXTerm);
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

    QPushButton *closeButton = new QPushButton("✕", topBar);
    QPushButton *fullscreenButton = new QPushButton("❐", topBar);
    
    topBarLayout->addWidget(fullscreenButton);
    topBarLayout->addStretch();
    topBarLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &TerminalWindow::close);
    connect(fullscreenButton, &QPushButton::clicked, this, &TerminalWindow::toggleFullScreen);

    xtermWidget = new QWidget(this);
    xtermWidget->setStyleSheet("background-color: black;");

    mainLayout->addWidget(topBar);
    mainLayout->addWidget(xtermWidget);
    setCentralWidget(centralWidget);
    setWindowTitle("Terminal Window");

    updateTopBarVisibility();
}

void TerminalWindow::launchXTerm() {
    WId winId = xtermWidget->winId();

    QString program = "xterm";
    QStringList arguments;
    arguments << "-into" << QString::number(winId) << "-geometry" << "80x24";

    xtermProcess = new QProcess(this);
    xtermProcess->start(program, arguments);
}

void TerminalWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (xtermProcess->state() == QProcess::Running) {
        xtermWidget->resize(event->size());
        QString command = QString("printf '\\e[8;%d;%dt' %1 %2")
                            .arg(xtermWidget->height())
                            .arg(xtermWidget->width());
        xtermProcess->write(command.toUtf8());
    }
}

void TerminalWindow::updateTopBarVisibility() {
    topBar->setVisible(!isFullScreenMode);
}

void TerminalWindow::closeEvent(QCloseEvent *event) {
    if (xtermProcess->state() == QProcess::Running) {
        xtermProcess->terminate();
        xtermProcess->waitForFinished();
    }
    QMainWindow::closeEvent(event);
}

void TerminalWindow::toggleFullScreen() {
    if (isFullScreenMode) {
        showNormal();
        isFullScreenMode = false;
    } else {
        showFullScreen();
        isFullScreenMode = true;
    }
    updateTopBarVisibility();
}

void TerminalWindow::focusInEvent(QFocusEvent *event) {
    QMainWindow::focusInEvent(event);
    if (xtermProcess->state() == QProcess::Running) {
        xtermWidget->setFocus();
    }
}
