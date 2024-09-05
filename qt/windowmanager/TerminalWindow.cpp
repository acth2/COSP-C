#include "TerminalWindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QDebug>

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), terminalProcess(new QProcess(this)) {

    setFixedSize(500, 500);

    setWindowFlags(Qt::Window);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    terminalProcess->start("xterm");

    if (!terminalProcess->waitForStarted()) {
        qDebug() << "Failed to start xterm process!";
    } else {
        qDebug() << "xterm started!";
    }
}

TerminalWindow::~TerminalWindow() {
    if (terminalProcess->state() == QProcess::Running) {
        terminalProcess->terminate();
        terminalProcess->waitForFinished();
    }
}
