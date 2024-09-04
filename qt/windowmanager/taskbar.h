#ifndef TASKBAR_H
#define TASKBAR_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QScreen>

class TaskBar : public QWidget {
    Q_OBJECT

public:
    explicit Taskbar(QWidget *parent = nullptr);
    explicit TaskBar(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *popup;
    QPushButton *startButton;
    void adjustSizeToScreen();
};

#endif // TASKBAR_H
