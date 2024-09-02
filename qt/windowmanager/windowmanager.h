#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QWidget>
#include <QEvent>

class WindowManager : public QWidget {
    Q_OBJECT

public:
    WindowManager(QWidget *parent = nullptr);
    ~WindowManager();

protected:
    bool event(QEvent *event) override;
};

#endif
