#ifndef TOPBAR_H
#define TOPBAR_H

#include <QtCore/qtextstream.h>
#include <QWidget>
#include <QWindow>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QProcess>
#include "../windowmanager.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>

class WindowManager;

class TopBar : public QWidget {
    Q_OBJECT

public:
    explicit TopBar(QWindow *parentWindow, WindowManager *manager, QWidget *parent = nullptr);
    void updatePosition();
    void updateTitle(const QString &title);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void closeRequested();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QWindow *trackedWindow;
    QLabel *titleLabel;
    QPushButton *closeButton;
    bool isDragging = false;
    QPoint dragStartPos;
    QPoint windowStartPos;
    WindowManager *windowManager;

    pid_t getProcessIdFromWindow(WId windowId);
};

#endif // TOPBAR_H
