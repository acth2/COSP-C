#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QTextEdit>
#include <QMouseEvent>
#include <QKeyEvent>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TerminalWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void toggleFullScreen();
    void windowedFullScreen();

private:
    QWidget *centralWidget;
    QWidget *topBar;
    QPushButton *closeButton;
    QPushButton *fullscreenButton;
    QTextEdit *terminalWidget;
    bool isFullScreenMode;
    bool dragging;
    bool isFullMode;
    bool windowedFull;
    QPoint dragStartPosition;

    void setupUI();
    void updateTopBarVisibility();
};

#endif // TERMINALWINDOW_H
