#ifndef TOPBAR_H
#define TOPBAR_H

#include <QWidget>
#include <QWindow>
#include <QLabel>
#include <QHBoxLayout>

class TopBar : public QWidget {
    Q_OBJECT

public:
    explicit TopBar(QWindow *parentWindow, QWidget *parent = nullptr);
    void updatePosition();
    void updateTitle(const QString &title);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QWindow *trackedWindow;
    QLabel *titleLabel;
    bool dragging = false;
    QPoint lastMousePos;
};

#endif // TOPBAR_H
