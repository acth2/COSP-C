#include "windowmanager.h"
#include <QDebug>

WindowManager::WindowManager(QWidget *parent)
    : QWidget(parent) {
}

WindowManager::~WindowManager() {

}

bool WindowManager::event(QEvent *event) {
    if (event->type() == QEvent::WindowActivate) {
        qDebug("Window activated");
    }
    return QWidget::event(event); 
}
