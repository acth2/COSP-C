#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QMouseEvent>

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

private:
    void setupUI();
    
    QWidget *centralWidget;
    QWidget *topBar;
    QPushButton *closeButton;
    QPushButton *fullscreenButton;
    QTextEdit *terminalWidget;
    bool isFullScreenMode;
    
    bool dragging;
    QPoint dragStartPosition;
};

#endif // TERMINALWINDOW_H
