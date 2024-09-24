#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QtCore/qtextstream.h>
#include "win/topbar.h"
#include <QWidget>
#include <QLabel>
#include <QSet>
#include <QTimer>
#include <QMap>
#include <QObject>
#include <QEvent>
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
    void updateTaskbarPosition(QWindow* window);
    void trackWindowEvents(Window xorgWindowId);
    void centerWindow(QWindow *window);
    void removeCloseButton(WId windowId);
    bool eventFilter(QObject *watched, QEvent *event) override;

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
    QLabel *leftSquare;
    QLabel *rightSquare;
    QLabel *bottomSquare;

    void createTrackingSquares();
    void updateTrackingSquares(QWindow*);

    void setupCloseButton(QWindow *window);
};

#endif // WINDOWMANAGER_H
