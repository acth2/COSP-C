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

enum ResizeDirection {
    NoResize,
    ResizeLeft,
    ResizeRight,
    ResizeBottom
};

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
    WindowManager *windowManager;
    QWindow *trackedWindow;
    QRect restoreGeometry;
    bool isMaximized;

    bool isDarkMode;
    bool isDragging;

    bool isResizingLeft;
    bool isResizingRight;
    bool isResizingBottom;
    QPoint resizeStartPos;
    QPoint dragStartPos; 


    bool isResizing;
    void updateCursorShape(const QPoint &localMousePos);
    void resizeWindow(const QPoint &mousePos);
    void startResizing(const QPoint &localMousePos);
    void stopResizing();
    bool isNearEdge(const QPoint &localMousePos, const QRect &windowGeometry, int margin = 10);

    enum ResizeDirection { None, Left, Right, Bottom, BottomLeft, BottomRight } resizeDirection;
    ResizeDirection getResizeDirection(const QPoint &localMousePos);

};

#endif // TOPBAR_H
