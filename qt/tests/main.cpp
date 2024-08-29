#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

// Un programme de test pour voir si Qt fonctionne..

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("COSP-C");
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *label = new QLabel("COSP-C CydraOSPrograms CODE");
    layout->addWidget(label);


    QPushButton *button1 = new QPushButton("COSP-C");
    label->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(button1);
    layout->addStretch();

    window.setLayout(layout);
    window.resize(320, 240);
    window.show();

    return app.exec();
}
