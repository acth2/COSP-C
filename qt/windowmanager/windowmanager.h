#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QWidget>
#include <QMap>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QScreen>
#include <X11/Xlib.h>
#include <QVBoxLayout>
#include "topbar.h"

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

    // These are the new method declarations you requested
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

    QTimer *resizeWindowCubesTimer;

signals:
    void cubePressed(WId windowId);

private slots:
    void enableResizeMode(WId windowId);
};

#endif // WINDOWMANAGER_H
