#ifndef TASKBAR_H
#define TASKBAR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QEvent>

class WindowManager;

class TaskBar : public QWidget {
    Q_OBJECT

public:
    explicit TaskBar(WindowManager *windowManager, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QLabel *popup;
    QPushButton *startButton;
    WindowManager *windowManager;
    bool popupVisible = false;
    void adjustSizeToScreen();
    void showPopup();
    void hidePopup();
    QLabel *popup;
    void installEventFilter();
    bool isPopupVisible = false;
};

#endif // TASKBAR_H
