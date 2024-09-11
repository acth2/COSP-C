#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QWidget>
#include <QLabel>
#include <QSet>
#include <QTimer>
#include <QMap>
#include "konami_code_handler.h"
#include "userinteractright.h"
#include "Window.h"

class WindowManager : public QWidget {
    Q_OBJECT

public:
    explicit WindowManager(QWidget *parent = nullptr);
    void appendLog(const QString &message);

protected:
    bool event(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void checkForNewWindows();

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
    QTimer *windowCheckTimer;
    Display *xDisplay;
    Window rootWindow;
};

#endif // WINDOWMANAGER_H
