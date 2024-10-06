#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QIcon>
#include <filesystem>
#include <iostream>
#include <QVBoxLayout>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QProcessEnvironment>

namespace fs = std::filesystem;

class DesktopIcons : public QWidget {
public:
    DesktopIcons(QWidget *parent = nullptr) : QWidget(parent) {
        createDesktopIcons();
    }

    void createDesktopIcons() {
        QString homeDir = QProcessEnvironment::systemEnvironment().value("HOME");
        QString desktopPath = homeDir + "/A2WM/Desktop";

        if (!fs::exists(desktopPath.toStdString())) {
            qDebug() << "Directory not found: " << desktopPath;
            return;
        }

        QGridLayout *gridLayout = new QGridLayout(this);
        int row = 0, col = 0;
        int maxCols = 5;

        for (const auto &entry : fs::directory_iterator(desktopPath.toStdString())) {
            QString filePath = QString::fromStdString(entry.path().string());
            QString fileName = QString::fromStdString(entry.path().filename().string());

            QPushButton *iconButton = new QPushButton(this);
            iconButton->setFixedSize(32, 32);
            iconButton->setIconSize(QSize(32, 32));

            if (entry.is_directory()) {
                iconButton->setIcon(QIcon("/usr/cydra/icons/folder.png"));
                connect(iconButton, &QPushButton::clicked, [=]() {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
                });
            } else if (entry.is_regular_file()) {
                iconButton->setIcon(QIcon("/usr/cydra/icons/file.png"));
                connect(iconButton, &QPushButton::clicked, [=]() {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
                });
            }

            QLabel *nameLabel = new QLabel(fileName, this);
            nameLabel->setStyleSheet("QLabel { color : white; }");

            gridLayout->addWidget(iconButton, row, col);
            gridLayout->addWidget(nameLabel, row + 1, col);

            col++;
            if (col >= maxCols) {
                col = 0;
                row += 2;
            }
        }

        this->setLayout(gridLayout);
    }
};
