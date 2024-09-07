#include "TerminalWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QProcess>
#include <QScreen>
#include <QCursor>
#include <QKeyEvent>
#include <QScrollBar>

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), isFullScreenMode(false), dragging(false), resizing(false) {
    setupUI();
    setCursor(Qt::ArrowCursor);
}

void TerminalWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F11) {
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
    } else if (event->key() == Qt::Key_Escape && isFullScreenMode) {
        isFullScreenMode = false;
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry.width() / 2, screenGeometry.height() / 2, 800, 600);
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // Execute command when Enter is pressed
        QString command = currentCommand.trimmed();
        executeCommand(command);
        currentCommand.clear();
    } else if (event->key() == Qt::Key_Backspace) {
        if (!currentCommand.isEmpty()) {
            currentCommand.chop(1);
            terminalWidget->setPlainText(currentText + currentCommand);
            terminalWidget->moveCursor(QTextCursor::End);
        }
    } else {
        currentCommand += event->text();
        terminalWidget->setPlainText(currentText + currentCommand);
        terminalWidget->moveCursor(QTextCursor::End);
    }
}

void TerminalWindow::executeCommand(const QString &command) {
    if (command == "clear") {
        terminalWidget->clear();
        currentText.clear();
    } else {
        QProcess process;
        process.start("/bin/bash", QStringList() << "-c" << command);
        process.waitForFinished();
        QString output = process.readAllStandardOutput();
        QString error = process.readAllStandardError();
        if (!output.isEmpty()) {
            currentText += output;
        }
        if (!error.isEmpty()) {
            currentText += error;
        }
        currentText += "\n$ ";
        terminalWidget->setPlainText(currentText);
        terminalWidget->moveCursor(QTextCursor::End);
    }
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
        if (onRightEdge || onBottomEdge) {
            resize(newSize);
        }
    } else if (dragging) {
        move(event->globalPos() - dragStartPosition);
    }

    QMainWindow::mouseMoveEvent(event);
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

void TerminalWindow::mouseReleaseEvent(QMouseEvent *event) {
    resizing = false;
    dragging = false;
    QMainWindow::mouseReleaseEvent(event);
}

void TerminalWindow::toggleFullScreen() {
    if (isFullMode) {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry.width() / 2, screenGeometry.height() / 2, 800, 600);
        isFullMode = false;
    } else {
        setGeometry(0, 0, 800, 500);
        isFullMode = true;
    }
    updateTopBarVisibility();
}

void TerminalWindow::updateTopBarVisibility() {
    topBar->setVisible(!isFullScreenMode);
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

    terminalWidget = new QPlainTextEdit(this);
    terminalWidget->setPlainText("$ ");
    terminalWidget->setReadOnly(true);

    currentText = "$ ";

    mainLayout->addWidget(topBar);
    mainLayout->addWidget(terminalWidget);

    setCentralWidget(centralWidget);
    setWindowTitle("Terminal Window");

    updateTopBarVisibility();
}
