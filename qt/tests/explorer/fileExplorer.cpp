#include <QApplication>
#include <QMainWindow>
#include <QTreeView>
#include <QListView>
#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QSplitter>

class FileExplorer : public QMainWindow {
    Q_OBJECT

public:
    FileExplorer(QWidget *parent = nullptr) : QMainWindow(parent) {
        model = new QFileSystemModel(this);
        model->setRootPath(QDir::rootPath());

        treeView = new QTreeView(this);
        treeView->setModel(model);
        treeView->setRootIndex(model->index(QDir::rootPath()));
        connect(treeView, &QTreeView::clicked, this, &FileExplorer::onTreeViewClicked);

        listView = new QListView(this);
        listView->setModel(model);
        listView->setRootIndex(model->index(QDir::rootPath()));

        QSplitter *splitter = new QSplitter(this);
        splitter->addWidget(treeView);
        splitter->addWidget(listView);

        setCentralWidget(splitter);

        QToolBar *toolBar = new QToolBar(this);
        addToolBar(Qt::TopToolBarArea, toolBar);

        QPushButton *refreshButton = new QPushButton("Rafraîchir", this);
        connect(refreshButton, &QPushButton::clicked, this, &FileExplorer::refresh);
        toolBar->addWidget(refreshButton);

        QLineEdit *searchEdit = new QLineEdit(this);
        toolBar->addWidget(searchEdit);

        QComboBox *viewComboBox = new QComboBox(this);
        viewComboBox->addItems({"Détails", "Icônes"});
        connect(viewComboBox, &QComboBox::currentTextChanged, this, &FileExplorer::changeView);
        toolBar->addWidget(viewComboBox);

        setWindowTitle("Explorateur de fichiers");
        resize(800, 600);
    }

public slots:
    void refresh() {
        model->setRootPath(QDir::rootPath());
        treeView->setRootIndex(model->index(QDir::rootPath()));
        listView->setRootIndex(model->index(QDir::rootPath()));
    }

    void changeView(const QString &viewType) {
        if (viewType == "Icônes") {
            listView->setViewMode(QListView::IconMode);
        } else {
            listView->setViewMode(QListView::ListMode);
        }
    }

    void onTreeViewClicked(const QModelIndex &index) {
        QString path = model->filePath(index);
        listView->setRootIndex(model->index(path));
    }

private:
    QFileSystemModel *model;
    QTreeView *treeView;
    QListView *listView;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    FileExplorer explorer;
    explorer.show();
    return app.exec();
}
