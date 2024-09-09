#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QProcess>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QFocusEvent>
#include <QCloseEvent>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TerminalWindow(QWidget *parent = nullptr);

private slots:
    void launchXTerm();
    void resizeXTerm();
    void toggleFullScreen();
    void windowedFullScreen();
    void executeCommand(const QString &command);

private:
    void setupUI();
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void updateTopBarVisibility();

    QWidget *centralWidget;
    QWidget *xtermWidget;
    QWidget *topBar;
    QPushButton *closeButton;
    QPushButton *fullscreenButton;
    QProcess *xtermProcess;
    bool isFullScreenMode;
    bool windowedFull;
    bool dragging;
    bool resizing;
    QPoint dragStartPosition;
    QPoint resizeStartPosition;
    QSize resizeStartSize;
};

#endif // TERMINALWINDOW_H
