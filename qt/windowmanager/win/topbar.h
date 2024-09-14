#ifndef TOPBAR_H
#define TOPBAR_H

#include <QWidget>
#include <QWindow>
#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>

class TopBar : public QWidget {
    Q_OBJECT

public:
    explicit TopBar(QWindow *parentWindow, QWidget *parent = nullptr);
    void updatePosition();
    void updateTitle(const QString &title);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QWindow *trackedWindow;
    QLabel *titleLabel;
    bool isDragging = false;
    QPoint lastMousePos;
};

#endif // TOPBAR_H
