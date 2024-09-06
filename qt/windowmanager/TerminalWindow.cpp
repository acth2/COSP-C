#include "TerminalWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QTextEdit>
#include <QPushButton>
#include <QDebug>
#include <QScreen>

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), isFullScreenMode(false), dragging(false), resizing(false) {
    setupUI();
    setMouseTracking(true);    
}

void TerminalWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F11) {
        if (isFullScreenMode) {
            showNormal();
            isFullScreenMode = false;
        } else {
            QScreen *screen = QApplication::primaryScreen();
            QRect screenGeometry = screen->geometry();
            setGeometry(0, 0, screenGeometry.width(), screenGeometry.height());
            showFullScreen();
            isFullScreenMode = true;
        }
        updateTopBarVisibility();
    } else if (event->key() == Qt::Key_Escape && isFullScreenMode) {
        isFullScreenMode = false;
        setGeometry(geometry().width() / 2, geometry().height() / 2, 800, 600);
    }
    QMainWindow::keyPressEvent(event);
}

void TerminalWindow::mouseMoveEvent(QMouseEvent *event) {
    const int resizeMargin = 8;
    QRect windowRect = rect();

    bool nearLeft = event->pos().x() <= resizeMargin;
    bool nearRight = event->pos().x() >= windowRect.width() - resizeMargin;
    bool nearTop = event->pos().y() <= resizeMargin;
    bool nearBottom = event->pos().y() >= windowRect.height() - resizeMargin;

    if (nearLeft || nearRight || nearTop || nearBottom) {
        if (nearLeft && nearBottom) {
            setCursor(Qt::SizeBDiagCursor);
        } else if (nearRight && nearBottom) {
            setCursor(Qt::SizeFDiagCursor);
        } else if (nearLeft && nearTop) {
            setCursor(Qt::SizeFDiagCursor);
        } else if (nearRight && nearTop) {
            setCursor(Qt::SizeBDiagCursor);
        } else if (nearLeft || nearRight) {
            setCursor(Qt::SizeHorCursor);
        } else if (nearTop || nearBottom) {
            setCursor(Qt::SizeVerCursor);
        }
        resizing = true;
    } else {
        setCursor(Qt::ArrowCursor);
        resizing = false;
    }

    if (resizing) {
        if (event->buttons() & Qt::LeftButton) {
            if (nearRight) {
                setGeometry(x(), y(), event->pos().x(), height());
            } else if (nearLeft) {
                int diff = event->pos().x();
                setGeometry(x() + diff, y(), width() - diff, height());
            } else if (nearBottom) {
                setGeometry(x(), y(), width(), event->pos().y());
            } else if (nearTop) {
                int diff = event->pos().y();
                setGeometry(x(), y() + diff, width(), height() - diff);
            }
        }
    }

    QMainWindow::mouseMoveEvent(event);
}

void TerminalWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = !resizing;
        dragStartPosition = event->globalPos() - frameGeometry().topLeft();
    }
    QMainWindow::mousePressEvent(event);
}

void TerminalWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = false;
        resizing = false;
    }
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
    connect(fullscreenButton, &QPushButton::clicked, this, &TerminalWindow::windowedFullScreen);

    terminalWidget = new QTextEdit(this);
    terminalWidget->setText("This is a simulated terminal.");
    terminalWidget->setReadOnly(true);

    mainLayout->addWidget(topBar);
    mainLayout->addWidget(terminalWidget);

    setCentralWidget(centralWidget);
    setWindowTitle("Terminal Window");

    updateTopBarVisibility();
}
