#ifndef KONAMI_CODE_HANDLER_H
#define KONAMI_CODE_HANDLER_H

#include <QObject>
#include <QKeyEvent>

class QLabel;

class KonamiCodeHandler : public QObject {
    Q_OBJECT

public:
    explicit KonamiCodeHandler(QLabel *logLabel, QObject *parent = nullptr);

    void keyPressEvent(QKeyEvent *event);

private:
    void appendLog(const QString &message);

    static const int konamiCode[];
    static const int konamiCodeLength;
    int currentCodeIndex;
    QLabel *logLabel;
};

#endif // KONAMI_CODE_HANDLER_H
