#include "TerminalWindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QDebug>

TerminalWindow::TerminalWindow(QWidget *parent)
    : QWidget(parent), terminalProcess(new QProcess(this)) {

    setFixedSize(500, 500);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *infoLabel = new QLabel("Terminal running xterm...", this);
    layout->addWidget(infoLabel);

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
