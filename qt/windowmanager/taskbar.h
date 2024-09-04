#ifndef TASKBAR_H
#define TASKBAR_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>

class TaskBar : public QWidget {
    Q_OBJECT

public:
    explicit TaskBar(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QLabel *popup;
    QPushButton *startButton;
    QPushButton *powerButton;
    void adjustSizeToScreen();
    void showPopup();
    void closePopup();
    void installEventFilter();
    void showPowerMenu();
    bool isPopupVisible = false;
};

#endif // TASKBAR_H
