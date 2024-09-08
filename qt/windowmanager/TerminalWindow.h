#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QProcess>
#include <QResizeEvent>
#include <QFocusEvent>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TerminalWindow(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

private slots:
    void toggleFullScreen();
    void windowedFullScreen();
    void executeCommand(const QString &command);

private:
    void setupUI();
    void launchXTerm();
    void updateTopBarVisibility();

    QWidget *topBar;
    QPushButton *closeButton;
    QPushButton *fullscreenButton;
    QProcess *terminalProcess;
    QWidget *centralWidget;
    QWidget *xtermWidget;
    QProcess *xtermProcess;
    bool isFullScreenMode;
    bool dragging;
    bool resizing;
    QPoint dragStartPosition;
    QSize resizeStartSize;
    QPoint resizeStartPosition;
    bool isFullMode;
    bool windowedFull;
    QString currentText;
    QString currentCommand;
};

#endif // TERMINALWINDOW_H
