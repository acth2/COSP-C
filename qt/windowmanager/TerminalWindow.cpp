#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QKeyEvent>
#include <QLabel>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    TerminalWindow(QWidget *parent = nullptr) : QMainWindow(parent), isFullScreenMode(false) {
        setupUI();
    }

protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_F11) {
            toggleFullScreen();
        } else if (event->key() == Qt::Key_Escape && isFullScreenMode) {
            toggleFullScreen();
        }
        QMainWindow::keyPressEvent(event);
    }

private slots:
    void toggleFullScreen() {
        if (isFullScreenMode) {
            showNormal();
            isFullScreenMode = false;
        } else {
            showFullScreen();
            isFullScreenMode = true;
        }
    }

private:
    void setupUI() {
        centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        QWidget *topBar = new QWidget(this);
        QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
        topBarLayout->setContentsMargins(5, 5, 5, 5);

        QPushButton *closeButton = new QPushButton("X", this);
        QPushButton *fullscreenButton = new QPushButton("[ ]", this);

        topBarLayout->addWidget(fullscreenButton);
        topBarLayout->addStretch();
        topBarLayout->addWidget(closeButton);

        connect(closeButton, &QPushButton::clicked, this, &TerminalWindow::close);
        connect(fullscreenButton, &QPushButton::clicked, this, &TerminalWindow::toggleFullScreen);

        mainLayout->addWidget(topBar);

        QLabel *label = new QLabel("Ceci est une fenêtre test sans xterm", this);
        mainLayout->addWidget(label);

        setCentralWidget(centralWidget);
    }

    QWidget *centralWidget;
    bool isFullScreenMode;
};
