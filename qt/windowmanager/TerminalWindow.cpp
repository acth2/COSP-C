#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QProcess>
#include <QTimer>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QScreen>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    TerminalWindow(QWidget *parent = nullptr);

private slots:
    void launchXTerm();
    void resizeXTerm();
    void toggleFullScreen();

private:
    void setupUI();
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    QWidget *centralWidget;
    QWidget *xtermWidget;
    QProcess *xtermProcess;
    QPushButton *closeButton;
    QPushButton *fullscreenButton;
    bool isFullScreenMode;
    QPoint dragStartPosition;
    bool dragging;
};

TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent), isFullScreenMode(false), dragging(false), xtermProcess(new QProcess(this)) {
    setupUI();
    QTimer::singleShot(100, this, &TerminalWindow::launchXTerm);

    // Set up a timer to resize xterm
    QTimer *resizeTimer = new QTimer(this);
    connect(resizeTimer, &QTimer::timeout, this, &TerminalWindow::resizeXTerm);
    resizeTimer->start(500); // Resize every 500ms
}

void TerminalWindow::setupUI() {
    centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    // Top bar for buttons
    QWidget *topBar = new QWidget(this);
    topBar->setFixedHeight(30);
    topBar->setStyleSheet("background-color: #333; color: white;");

    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    closeButton = new QPushButton("✕", topBar);
    fullscreenButton = new QPushButton("❐", topBar);

    topBarLayout->addWidget(fullscreenButton);
    topBarLayout->addStretch();
    topBarLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &TerminalWindow::close);
    connect(fullscreenButton, &QPushButton::clicked, this, &TerminalWindow::toggleFullScreen);

    layout->addWidget(topBar);

    // xterm widget (this is where xterm will be embedded)
    xtermWidget = new QWidget(this);
    layout->addWidget(xtermWidget);

    setCentralWidget(centralWidget);
    setWindowTitle("Terminal Window");
}

void TerminalWindow::launchXTerm() {
    WId winId = xtermWidget->winId();

    QString program = "xterm";
    QStringList arguments;
    arguments << "-into" << QString::number(winId) << "-geometry" << "80x24";

    xtermProcess->start(program, arguments);
}

void TerminalWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    resizeXTerm();
}

void TerminalWindow::resizeXTerm() {
    if (xtermProcess->state() == QProcess::Running) {
        int columns = this->width() / 9;
        int rows = this->height() / 18;

        QString resizeCommand = QString("printf '\\e[8;%1;%2t'").arg(rows).arg(columns);
        xtermProcess->write(resizeCommand.toUtf8());
        xtermProcess->waitForBytesWritten();
    }
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
}

void TerminalWindow::mousePressEvent(QMouseEvent *event) {
    dragging = true;
    dragStartPosition = event->globalPos() - frameGeometry().topLeft();
}

void TerminalWindow::mouseMoveEvent(QMouseEvent *event) {
    if (dragging) {
        move(event->globalPos() - dragStartPosition);
    }
}

void TerminalWindow::mouseReleaseEvent(QMouseEvent *event) {
    dragging = false;
}
