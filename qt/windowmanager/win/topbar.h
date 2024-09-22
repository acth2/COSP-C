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
#include <QRect>

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
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void closeRequested();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void closeTrackedWindow();
    void maximizeWindow();
    void toggleMaximizeRestore();

private:
    QLabel *titleLabel;
    QLabel *popup;
    QPushButton *closeButton;
    QPushButton *maximizeButton;
    bool isDragging = false;
    QPoint dragStartPos;
    QPoint windowStartPos;

    enum ResizeRegion {
        NoResize,
        TopEdge,
        BottomEdge,
        LeftEdge,
        RightEdge,
        TopLeftCorner,
        TopRightCorner,
        BottomLeftCorner,
        BottomRightCorner
    };

    const int EDGE_THRESHOLD = 5;
    ResizeRegion currentResizeRegion = NoResize;

    void detectEdge(const QPoint &pos);
    void startResize();
    QCursor cursorForResizeRegion(ResizeRegion region) const;
    WindowManager *windowManager;
    QWindow *trackedWindow;
    QRect restoreGeometry;
    bool isMaximized;

    bool isDarkMode;
};

#endif // TOPBAR_H
