#include <QApplication>
#include <QMainWindow>
#include <QTreeView>
#include <QFileSystemModel>
#include <QVBoxLayout>

class FileExplorer : public QMainWindow {
public:
    FileExplorer(QWidget *parent = nullptr) : QMainWindow(parent) {
        QFileSystemModel *model = new QFileSystemModel(this);
        model->setRootPath(QDir::rootPath());

        QTreeView *treeView = new QTreeView(this);
        treeView->setModel(model);
        treeView->setRootIndex(model->index(QDir::rootPath()));
        
        setCentralWidget(treeView);
        setWindowTitle("File Explorer");
        resize(800, 600);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    FileExplorer explorer;
    explorer.show();
    return app.exec();
}
