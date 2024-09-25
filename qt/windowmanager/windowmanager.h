#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QtCore/qtextstream.h>
#include "win/topbar.h"
#include <QWidget>
#include <QLabel>
#include <QSet>
#include <QTimer>
#include <QMap>
#include <QMouseEvent>
#include <QResizeEvent>
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
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void onRightSquarePressed(QMouseEvent *event, WId windowId);
    void onRightSquareMoved(QMouseEvent *event, WId windowId);
    void onBottomSquarePressed(QMouseEvent *event, WId windowId);
    void onBottomSquareMoved(QMouseEvent *event, WId windowId);
    void onSquareReleased(QMouseEvent *event);
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
    QTimer *resizeWindowCubesTimer;
    struct TrackingSquares {
        QLabel *leftSquare;
        QLabel *rightSquare;
        QLabel *bottomSquare;
    };

    QLabel *leftSquare;
    QLabel *rightSquare;
    QLabel *bottomSquare;
    QMap<WId, TrackingSquares> windowSquares;

    void createTrackingSquares(WId windowId);
    void updateTrackingSquares(WId windowId);

    void setupCloseButton(QWindow *window);
    void setSupportingWMCheck();

    bool resizing = false;
    WId currentWindowId;
    QPoint lastMousePosition;
    void resizeWindow(WId windowId, int widthDelta, int heightDelta);

    void onLoop();
    Window root;
};

#endif // WINDOWMANAGER_H
