#ifndef SETTINGS_H
#define SETTINGS_H
#include <QApplication>
#include <QString>
#include <cstdint>

struct Settings{
    QString mask = ".txt";
    bool isOverwriting = 1;
    bool needDelete = 0;
    bool needTimer = 0;
    int timerTime = 0;
    QByteArray key;
    QString dir = QCoreApplication::applicationDirPath();
    QString dir2 = QCoreApplication::applicationDirPath(); //директория выходных файлов
};

#endif // SETTINGS_H
