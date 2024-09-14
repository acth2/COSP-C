#ifndef CLOSEBUTTON_H
#define CLOSEBUTTON_H

#include <QWidget>
#include <QWindow>

class CloseButton : public QWidget
{
    Q_OBJECT

public:
    explicit CloseButton(QWindow *trackedWindow, QWidget *parent = nullptr);
    void updatePosition();

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QWindow *trackedWindow;

signals:
    void closeButtonClicked();
    void closeRequested();

private slots:
    void handleButtonClicked();
};

#endif // CLOSEBUTTON_H
