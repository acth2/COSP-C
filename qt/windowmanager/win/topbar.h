#ifndef TOPBAR_H
#define TOPBAR_H

#include <QWidget>
#include <QWindow>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QProcess>
#include <QPaintEvent>
#include <QEvent>
#include <QObject>
#include <QMouseEvent>

class WindowManager;

class TopBar : public QWidget {
    Q_OBJECT

public:
    explicit TopBar(QWindow *parentWindow, WindowManager *manager, QWidget *parent = nullptr);
    void updatePosition();
    void updateTitle(const QString &title);
    QWindow* getTrackedWindow() const;
    QLabel* getPopup() const;
    void closePopup();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void closeRequested();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void closeTrackedWindow();

private:
    QLabel *titleLabel;
    QLabel *popup;
    QPushButton *closeButton;
    bool isDragging = false;
    QPoint dragStartPos;
    QPoint windowStartPos;
    WindowManager *windowManager;
    QWindow *trackedWindow;
};

#endif // TOPBAR_H
