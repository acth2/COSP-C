#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget mainWindow;
    mainWindow.setGeometry(0, 0, 800, 600);  // test
    mainWindow.show();

    return app.exec();
}
