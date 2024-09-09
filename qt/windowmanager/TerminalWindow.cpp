#include "TerminalWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QProcess>
#include <QScreen>
#include <QCursor>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QTimer>
#include <QResizeEvent>
#include <QDebug>  // Added for debugging

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), 
      isFullScreenMode(false), 
      dragging(false), 
      resizing(false),
      xtermProcess(new QProcess(this)) {
    setupUI();
    setCursor(Qt::ArrowCursor);

    qDebug() << "TerminalWindow initialized";
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

    closeButton = new QPushButton("✕", topBar);
    fullscreenButton = new QPushButton("❐", topBar);

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
    if (!xtermWidget) {
        qDebug() << "xtermWidget is null!";
        return;
    }

    WId winId = xtermWidget->winId();
    QString program = "xterm";
    QStringList arguments;
    arguments << "-into" << QString::number(winId) << "-geometry" << "80x24";

    qDebug() << "Launching xterm with arguments:" << arguments;

    xtermProcess->start(program, arguments);
    if (!xtermProcess->waitForStarted()) {
        qDebug() << "Failed to start xterm!";
    }
}

void TerminalWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (xtermProcess->state() == QProcess::Running) {
        resizeXTerm();
    }
}

void TerminalWindow::resizeXTerm() {
    if (xtermProcess->state() == QProcess::Running) {
        int newColumns = xtermWidget->width() / 9;
        int newRows = xtermWidget->height() / 18;

        QString resizeCommand = QString("printf '\\e[8;%1;%2t'").arg(newRows).arg(newColumns);
        xtermProcess->write(resizeCommand.toUtf8());
        xtermProcess->waitForBytesWritten();
    }
}

void TerminalWindow::mousePressEvent(QMouseEvent *event) {
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

void TerminalWindow::mouseMoveEvent(QMouseEvent *event) {
    int margin = 35;
    int marginIconing = 20;

    bool onRightEdge = event->x() > (width() - marginIconing);
    bool onBottomEdge = event->y() > (height() - marginIconing);

    if (onRightEdge && onBottomEdge) {
        setCursor(Qt::SizeFDiagCursor);
    } else if (onRightEdge) {
        setCursor(Qt::SizeHorCursor);
    } else if (onBottomEdge) {
        setCursor(Qt::SizeVerCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }

    if (resizing) {
        QSize newSize = resizeStartSize + QSize(event->globalPos().x() - resizeStartPosition.x(),
                                                event->globalPos().y() - resizeStartPosition.y());
        resize(newSize);
    } else if (dragging) {
        move(event->globalPos() - dragStartPosition);
    }
    QMainWindow::mouseMoveEvent(event);
}

void TerminalWindow::mouseReleaseEvent(QMouseEvent *event) {
    resizing = false;
    dragging = false;
    QMainWindow::mouseReleaseEvent(event);
}

void TerminalWindow::toggleFullScreen() {
    if (isFullScreenMode) {
        showNormal();
        isFullScreenMode = false;
    } else {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry);
        showFullScreen();
        isFullScreenMode = true;
    }
    updateTopBarVisibility();
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

void TerminalWindow::focusInEvent(QFocusEvent *event) {
    if (xtermProcess && xtermProcess->state() == QProcess::Running) {
        xtermWidget->setFocus(); 
    }
    QMainWindow::focusInEvent(event);
}
