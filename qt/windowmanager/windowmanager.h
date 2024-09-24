#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QtCore/qtextstream.h>
#include "win/topbar.h"
#include <QWidget>
#include <QLabel>
#include <QSet>
#include <QTimer>
#include <QMap>
#include "taskbar.h"
#include "konami_code_handler.h"
#include "userinteractright.h"
#include <X11/Xlib.h>

class TopBar;

class WindowManager : public QWidget {
    Q_OBJECT

public:
    explicit WindowManager(QWidget *parent = nullptr);
    void appendLog(const QString &message);
    QMap<WId, TopBar*> windowTopBars;
    void closeWindow(WId xorgWindowId);

protected:
    bool event(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void updateTaskbarPosition(QWindow* window);
    void trackWindowEvents(Window xorgWindowId);
    void centerWindow(QWindow *window);
    void removeCloseButton(WId windowId);

private slots:
    void checkForNewWindows();
    void toggleConsole();
    void processX11Events();
    void cleanUpClosedWindows();

private:

    QString backgroundImagePath;
    QLabel *logLabel;
    QSet<QString> loggedMessages;
    KonamiCodeHandler *konamiCodeHandler;
    bool isConsoleVisible;
    UserInteractRight *userInteractRightWidget;

    void createAndTrackWindow(WId xorgWindowId);
    void listExistingWindows();
    QMap<WId, QWindow*> trackedWindows;
    QMap<QWindow*, TaskBar*> windowTaskbars;
    QTimer *windowCheckTimer;

    QMap<WId, QSize> windowOriginalSizes;

    void setupCloseButton(QWindow *window);
};

class CustomContainerWidget : public QWidget {
    Q_OBJECT

public:
    CustomContainerWidget(QWidget *parent = nullptr) : QWidget(parent) {
        leftSquare = new QWidget(this);
        leftSquare->setFixedSize(squareSize, squareSize);
        leftSquare->setStyleSheet("background-color: red;");

        rightSquare = new QWidget(this);
        rightSquare->setFixedSize(squareSize, squareSize);
        rightSquare->setStyleSheet("background-color: green;");

        bottomSquare = new QWidget(this);
        bottomSquare->setFixedSize(squareSize, squareSize);
        bottomSquare->setStyleSheet("background-color: blue;");
    }

    void setSquareSize(int size) {
        squareSize = size;
        leftSquare->setFixedSize(squareSize, squareSize);
        rightSquare->setFixedSize(squareSize, squareSize);
        bottomSquare->setFixedSize(squareSize, squareSize);
        updateSquaresPosition();
    }

protected:
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);
        updateSquaresPosition();
    }

    void moveEvent(QMoveEvent *event) override {
        QWidget::moveEvent(event);
        updateSquaresPosition();
    }

private:
    void updateSquaresPosition() {
        leftSquare->move(0, (height() - squareSize) / 2);
        rightSquare->move(width() - squareSize, (height() - squareSize) / 2);
        bottomSquare->move((width() - squareSize) / 2, height() - squareSize);
    }

    QWidget *leftSquare;
    QWidget *rightSquare;
    QWidget *bottomSquare;
    int squareSize = 20;
};

#endif // WINDOWMANAGER_H
