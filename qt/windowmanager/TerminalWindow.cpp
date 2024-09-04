#include "TerminalWindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QProcess>
#include <QX11Info>

TerminalWindow::TerminalWindow(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    containerWidget = new QWidget(this);
    layout->addWidget(containerWidget);
    setLayout(layout);

    terminalProcess = new QProcess(this);
    
    QStringList arguments;
    arguments << "-into" << QString::number(containerWidget->winId());
    
    terminalProcess->start("xterm", arguments);
}

TerminalWindow::~TerminalWindow() {
    if (terminalProcess) {
        terminalProcess->terminate();
        terminalProcess->waitForFinished();
    }
}
