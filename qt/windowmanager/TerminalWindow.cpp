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
#include <QDebug>

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), xtermProcess(new QProcess(this)) {
    setupUI();
    setCursor(Qt::ArrowCursor);

    startXterm();
}

void TerminalWindow::setupUI() {
    centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    xtermWidget = new QWidget(this);
    xtermWidget->setStyleSheet("background-color: black;");

    mainLayout->addWidget(xtermWidget);

    setCentralWidget(centralWidget);
    setWindowTitle("Terminal Window");

    updateTopBarVisibility();
}

void TerminalWindow::launchXTerm() {
    WId winId = terminalWidget->winId();

    QString program = "xterm";
    QStringList arguments;
    arguments << "-into" << QString::number(winId) << "-geometry" << "80x24";

    terminalProcess = new QProcess(this);
    terminalProcess->start(program, arguments);
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
