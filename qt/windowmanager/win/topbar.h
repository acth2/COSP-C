#ifndef TOPBAR_H
#define TOPBAR_H

#include <QWidget>
#include <QWindow>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QProcess>
#include <QEvent>
#include <QObject>
#include "../windowmanager.h"

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
    bool eventFilter(QObject *obj, QEvent *event) override;
    QWindow *trackedWindow;

signals:
    void closeRequested();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void closeTrackedWindow();

private:
    QLabel *titleLabel;
    QPushButton *closeButton;
    bool isDragging = false;
    QPoint dragStartPos;
    QPoint windowStartPos;
    WindowManager *windowManager;
};

#endif // TOPBAR_H
