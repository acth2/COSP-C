#ifndef X11WINDOWMANAGER_H
#define X11WINDOWMANAGER_H

#include <QObject>
#include <QWindow>
#include "x11windowcontainer.h"

class X11WindowManager : public QObject {
    Q_OBJECT

public:
    explicit X11WindowManager(QObject *parent = nullptr);
    void startAndEmbedX11Window();

private:
    QWindow *createX11Window();
    X11WindowContainer *x11Container;
};

#endif // X11WINDOWMANAGER_H
