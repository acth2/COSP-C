#include <QWidget>
#include <QWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

class X11WindowContainer : public QWidget {
    Q_OBJECT

public:
    explicit X11WindowContainer(QWidget *parent = nullptr);
    void embedX11Window(WId x11WindowId);

private:
    QWindow *x11Window;
    QWidget *windowContainer;
};

X11WindowContainer::X11WindowContainer(QWidget *parent)
    : QWidget(parent), x11Window(nullptr) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    windowContainer = new QWidget(this);
    layout->addWidget(windowContainer);
    setLayout(layout);
}

void X11WindowContainer::embedX11Window(WId x11WindowId) {
    if (x11Window) {
        delete x11Window;
    }

    x11Window = QWindow::fromWinId(x11WindowId);
    QWidget *container = QWidget::createWindowContainer(x11Window, this);
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(this->layout());
    layout->addWidget(container);
    container->setFocus();
}
