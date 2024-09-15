#ifndef CLOSEBUTTON_H
#define CLOSEBUTTON_H

#include <QPushButton>
#include "../../windowmanager.h"
#include <QWindow>

class CloseButton : public QPushButton {
    Q_OBJECT

public:
    explicit CloseButton(QWindow *trackedWindow, QWidget *parent = nullptr);

    void updatePosition();

signals:
    void closeRequested();

private slots:
    void handleButtonClicked();

private:
    QWindow *trackedWindow;
};

#endif // CLOSEBUTTON_H
