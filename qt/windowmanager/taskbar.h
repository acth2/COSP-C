#ifndef TASKBAR_H
#define TASKBAR_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QScreen>
#include <QLabel>

class TaskBar : public QWidget {
    Q_OBJECT

public:
    explicit TaskBar(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void showPopup();

private:
    QLabel *popup;
    QPushButton *startButton;
    void adjustSizeToScreen();
};

#endif // TASKB
