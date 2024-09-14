#ifndef TOPBAR_H
#define TOPBAR_H

#include <QWidget>
#include <QWindow>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

class TopBar : public QWidget {
    Q_OBJECT

public:
    explicit TopBar(QWindow *parentWindow, QWidget *parent = nullptr);
    void updatePosition();
    void updateTitle(const QString &title);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void closeRequested();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void handleCloseButtonClicked();

private:
    QWindow *trackedWindow;
    QLabel *titleLabel;
    QPushButton *closeButton;
    bool isDragging = false;
    QPoint dragStartPos;
    QPoint windowStartPos;
};

#endif // TOPBAR_H
