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

void TerminalWindow::startXterm() {
    QStringList arguments;
    arguments << "-into" << QString::number(xtermWidget->winId());
    arguments << "-geometry" << QString("%dx%d+0+0").arg(xtermWidget->width()).arg(xtermWidget->height());

    xtermProcess->start("xterm", arguments);

    if (!xtermProcess->waitForStarted()) {
        qWarning() << "Failed to start xterm!";
    }
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

void TerminalWindow::closeEvent(QCloseEvent *event) {
    if (xtermProcess->state() == QProcess::Running) {
        xtermProcess->terminate();
        xtermProcess->waitForFinished();
    }
    QMainWindow::closeEvent(event);
}
