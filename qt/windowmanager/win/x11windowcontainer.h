#ifndef X11WINDOWCONTAINER_H
#define X11WINDOWCONTAINER_H

#include <QWidget>
#include <QWindow>

class X11WindowContainer : public QWidget {
    Q_OBJECT

public:
    explicit X11WindowContainer(QWindow *window, QWidget *parent = nullptr);
    void updateWindowGeometry();

private:
    QWindow *x11Window;
};

#endif // X11WINDOWCONTAINER_H
