#ifndef DESKTOPICONS_H
#define DESKTOPICONS_H

#include <QWidget>
#include <QDir>
#include <QList>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class DesktopIcons : public QWidget {
    Q_OBJECT

public:
    explicit DesktopIcons(QWidget *parent = nullptr);
    void loadDesktopItems();
    void setupIcon(const QString &filePath, const QString &fileName);

private:
    QVBoxLayout *layout;
    QString desktopPath;
    QString iconPath;
};

#endif // DESKTOPICONS_H
