#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QIcon>
#include <QProcessEnvironment>
#include <QDebug>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
class DesktopIcons : public QWidget {
public:
    DesktopIcons(QWidget *parent = nullptr) : QWidget(parent) {
        createDesktopIcons();
    }

    void createDesktopIcons() {
        QString homeDir = QProcessEnvironment::systemEnvironment().value("HOME");
        QString desktopPath = homeDir + "/A2WM/Desktop";

        DIR *dir = opendir(desktopPath.toUtf8().constData());
        if (!dir) {
            qDebug() << "Directory not found: " << desktopPath;
            return;
        }

        struct dirent *entry;
        QGridLayout *gridLayout = new QGridLayout();
        int row = 0, col = 0;
        int maxCols = 5;

        while ((entry = readdir(dir)) != nullptr) {
            QString fileName = QString::fromUtf8(entry->d_name);
            if (fileName == "." || fileName == "..")
                continue;

            QString filePath = desktopPath + "/" + fileName;

            struct stat fileStat;
            if (stat(filePath.toUtf8().constData(), &fileStat) == -1) {
                continue;
            }

            QPushButton *iconButton = new QPushButton(this);
            iconButton->setFixedSize(32, 32);
            iconButton->setIconSize(QSize(32, 32));

            if (S_ISDIR(fileStat.st_mode)) {
                iconButton->setIcon(QIcon("/usr/cydra/icons/folder.png"));
                connect(iconButton, &QPushButton::clicked, [filePath]() {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
                });
            } else if (S_ISREG(fileStat.st_mode)) {
                iconButton->setIcon(QIcon("/usr/cydra/icons/file.png"));
                connect(iconButton, &QPushButton::clicked, [filePath]() {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
                });
            } else {
                continue;
            }

            QLabel *nameLabel = new QLabel(fileName, this);
            nameLabel->setStyleSheet("QLabel { color : white; }");
            nameLabel->setAlignment(Qt::AlignCenter);

            QVBoxLayout *iconLayout = new QVBoxLayout();
            iconLayout->addWidget(iconButton, 0, Qt::AlignCenter);
            iconLayout->addWidget(nameLabel, 0, Qt::AlignCenter);

            QWidget *iconWidget = new QWidget(this);
            iconWidget->setLayout(iconLayout);

            gridLayout->addWidget(iconWidget, row, col);

            col++;
            if (col >= maxCols) {
                col = 0;
                row++;
            }
        }

        closedir(dir);

        // Set the grid layout
        this->setLayout(gridLayout);
    }
};
