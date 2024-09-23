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
#include <QResizeEvent>
#include <QFocusEvent>
#include <QCloseEvent>

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
    void resizeEvent(QResizeEvent *event) override; // For handling resize events
    void closeEvent(QCloseEvent *event) override;   // For handling window close
    void focusInEvent(QFocusEvent *event) override; // To handle focus changes

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
    WindowManager *windowManager;
    QWindow *trackedWindow;
    QRect restoreGeometry;
    bool isMaximized;

    bool isDarkMode;

    QWidget *rightResizeHandle;
    QWidget *leftResizeHandle;
    QWidget *bottomResizeHandle;
    QSize resizeStartSize;
    QPoint resizeStartPosition;
    bool dragging;
    bool resizing;
    
    bool isFullScreenMode = false;
    bool windowedFull = false;
    QPoint dragStartPosition;
    QSize resizeStartSize;
    QPoint resizeStartPosition;
    void setupUI();
};

#endif // TOPBAR_H
