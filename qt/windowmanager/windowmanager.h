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

struct TrackingSquares {
    QLabel *leftSquare;
    QLabel *rightSquare;
    QLabel *bottomSquare;
};

class KonamiCodeHandler;

class WindowManager : public QWidget {
    Q_OBJECT

public:
    explicit WindowManager(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *qtEvent) override;

    bool eventFilter(QObject *object, QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void setSupportingWMCheck();
    void listExistingWindows();
    void checkForNewWindows();
    void trackWindowEvents(Window xorgWindowId);
    void processX11Events();
    void createAndTrackWindow(WId xorgWindowId);
    void createTrackingSquares(WId windowId);
    void updateTrackingSquares(WId windowId);
    void updateTaskbarPosition(QWindow *window);
    void cleanUpClosedWindows();
    void appendLog(const QString &message);
    void toggleConsole();
    void closeWindow(WId windowId);

    Display *xDisplay;
    QMap<WId, QWindow *> trackedWindows;
    QMap<WId, TopBar *> windowTopBars;
    QMap<WId, TrackingSquares> windowSquares;
    QLabel *logLabel;
    QString backgroundImagePath;
    KonamiCodeHandler *konamiCodeHandler;
    QTimer *windowCheckTimer;
    QSet<QString> loggedMessages;
    QWidget *userInteractRightWidget;
    bool isConsoleVisible;
    bool resizeMode;
    QPoint lastMousePosition;

    QTimer *resizeWindowCubesTimer;

signals:
    void cubePressed(WId windowId);

private slots:
    void enableResizeMode(WId windowId);
};

#endif // WINDOWMANAGER_H
