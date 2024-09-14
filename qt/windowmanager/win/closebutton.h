#ifndef CLOSEBUTTON_H
#define CLOSEBUTTON_H

#include <QWidget>
#include <QWindow>

class CloseButton : public QWidget
{
    Q_OBJECT

public:
    CloseButton(QWindow *parentWindow, QWidget *parent = nullptr);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QWindow *trackedWindow;

signals:
    void closeButtonClicked();
};

#endif // CLOSEBUTTON_H
