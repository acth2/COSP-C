#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QWidget>
#include <QLabel>
#include <QSet>
#include <X11/Xlib.h>
#include "konami_code_handler.h"
#include "userinteractright.h"
#include "Window.h"

class WindowManager : public QWidget {
    Q_OBJECT

public:
    explicit WindowManager(QWidget *parent = nullptr);
    void appendLog(const QString &message);
    void attachTaskbarToWindow(WId xorgWindowId);

protected:
    bool event(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QString backgroundImagePath;
    QLabel *logLabel;
    QSet<QString> loggedMessages;
    KonamiCodeHandler *konamiCodeHandler;
    bool isConsoleVisible;
    UserInteractRight *userInteractRightWidget;

    Display *display;
    Window root;

    void monitorXorgWindows();
    void attachTaskbarToWindow(WId xorgWindowId);
    void handleNewXorgWindow(Window xorgWindowId);

    void toggleConsole();
};

#endif // WINDOWMANAGER_H
