#ifndef KONAMI_CODE_HANDLER_H
#define KONAMI_CODE_HANDLER_H

#include <QObject>
#include <QKeyEvent>
#include <QList>
#include <QTimer>

class KonamiCodeHandler : public QObject {
    Q_OBJECT

public:
    explicit KonamiCodeHandler(QObject *parent = nullptr);

signals:
    void konamiCodeEntered();

public slots:
    void handleKeyPress(QKeyEvent *event);

private:
    QList<int> currentSequence;
    const QList<int> konamiCode = {Qt::Key_Up, Qt::Key_Up, Qt::Key_Down, Qt::Key_Down, 
                                   Qt::Key_Left, Qt::Key_Right, Qt::Key_Left, Qt::Key_Right, 
                                   Qt::Key_B, Qt::Key_A};
    QTimer *resetTimer;

    void resetSequence();
};

#endif // KONAMI_CODE_HANDLER_H
