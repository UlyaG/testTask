#ifndef TASKPROCESS_H
#define TASKPROCESS_H
#include <QQueue>
#include <QObject>
#include <QMutexLocker>
#include <QFile>
#include <QFileInfo>
#include <QProgressBar>
#include "Settings.h"

struct Task {
    Settings sett;
    QStringList files;
};

class TaskProcess : public QObject {
    Q_OBJECT
public:
    TaskProcess(QProgressBar* progressBar1, QProgressBar* progressBar2);
public:
    void getTask(Task task);
public slots:
    void startProcessing();

signals:
    void processNext();
private:
    QQueue<Task> queue;
    bool isProcessing;
    QMutex mutex;
    QProgressBar* bar1;
    QProgressBar* bar2;
    void fileProcess(Settings sett, QString fileName);
    QString uniqFileName(QString originalPath);
};

#endif // TASKPROCESS_H
