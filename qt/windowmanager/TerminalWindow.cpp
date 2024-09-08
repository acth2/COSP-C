#include "TerminalWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QProcess>
#include <QScreen>
#include <QCursor>
#include <QKeyEvent>
#include <QScrollBar>
#include <QFile>
#include <QSocketNotifier>
#include <pty.h>
#include <unistd.h>

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
        QString command = currentCommand.trimmed();
        if (!command.isEmpty()) {
            executeCommand(command);
            currentCommand.clear();
        }
    } else if (event->key() == Qt::Key_Backspace) {

        if (!currentCommand.isEmpty()) {
            currentCommand.chop(1);
            terminalWidget->setPlainText(currentText + currentCommand);
            terminalWidget->moveCursor(QTextCursor::End);
        }
    } else if (event->matches(QKeySequence::Copy)) {
        terminalWidget->copy();
    } else if (event->matches(QKeySequence::Paste)) {
        QString clipboardText = QApplication::clipboard()->text();
        currentCommand += clipboardText;
        terminalWidget->setPlainText(currentText + currentCommand);
        terminalWidget->moveCursor(QTextCursor::End);
    } else if (event->key() == Qt::Key_Space) {
        currentCommand += ' ';
        terminalWidget->setPlainText(currentText + currentCommand);
        terminalWidget->moveCursor(QTextCursor::End);
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
        int master_fd, slave_fd;
        char slaveName[100];
        if (openpty(&master_fd, &slave_fd, slaveName, NULL, NULL) == -1) {
            qWarning("Unable to create PTY");
            return;
        }

        pid_t pid = fork();
        if (pid == 0) {
            close(master_fd);
            login_tty(slave_fd);
            execl("/bin/bash", "bash", "-c", command.toUtf8().constData(), NULL);
            exit(0);
        } else {
            close(slave_fd);
            QSocketNotifier *notifier = new QSocketNotifier(master_fd, QSocketNotifier::Read, this);
            connect(notifier, &QSocketNotifier::activated, [this, master_fd]() {
                char buffer[1024];
                ssize_t bytesRead = read(master_fd, buffer, sizeof(buffer) - 1);
                if (bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    currentText += QString::fromUtf8(buffer);
                    terminalWidget->setPlainText(currentText);
                    terminalWidget->moveCursor(QTextCursor::End);
                }
            });
        }
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

void TerminalWindow::windowedFullScreen() {
    if (!windowedFull) {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry);
        windowedFull = true;
    } else {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        setGeometry(screenGeometry.width() / 2, screenGeometry.height() / 2, 350, 350);
        windowedFull = false;
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
    terminalWidget->setReadOnly(true);

    QProcess ps1Process;
    ps1Process.start("/bin/bash", QStringList() << "-c" << "echo $PS1");
    ps1Process.waitForFinished();
    QString ps1 = ps1Process.readAllStandardOutput().trimmed();
    
    currentText = ps1 + " ";
    terminalWidget->setPlainText(currentText);

    mainLayout->addWidget(topBar);
    mainLayout->addWidget(terminalWidget);

    setCentralWidget(centralWidget);
    setWindowTitle("Terminal Window");

    updateTopBarVisibility();
}

