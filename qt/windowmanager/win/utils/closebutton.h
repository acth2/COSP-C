#ifndef CLOSEBUTTON_H
#define CLOSEBUTTON_H

#include <QtCore/qtextstream.h>
#include <QWidget>
#include <QPushButton>
#include <QWindow>
#include <QMap>
#include <X11/Xlib.h>

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
    QMap<WId, CloseButton*> closeButtons;
};

#endif // CLOSEBUTTON_H
