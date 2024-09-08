#include "TerminalWindow.h"
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QDebug>
#include <QTimer>

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), xtermProcess(new QProcess(this)) {
    setupUI();
    setCursor(Qt::ArrowCursor);
        
    QTimer::singleShot(100, this, &TerminalWindow::launchXTerm);
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
}

void TerminalWindow::launchXTerm() {
    WId winId = xtermWidget->winId();

    QString program = "xterm";
    QStringList arguments;
    arguments << "-into" << QString::number(winId) << "-geometry" << "80x24";

    xtermProcess->start(program, arguments);
    if (!xtermProcess->waitForStarted()) {
        qDebug() << "Failed to start xterm";
    }
}

void TerminalWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (xtermProcess->state() == QProcess::Running) {
        xtermWidget->resize(event->size());
    }
}

void TerminalWindow::closeEvent(QCloseEvent *event) {
    if (xtermProcess->state() == QProcess::Running) {
        xtermProcess->terminate();
        xtermProcess->waitForFinished();
    }
    QMainWindow::closeEvent(event);
}
