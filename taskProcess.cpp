#include "taskProcess.h"

TaskProcess::TaskProcess(QProgressBar* progressBar1, QProgressBar* progressBar2){
    bar1 = progressBar1;
    bar2 = progressBar2;
    isProcessing=0;
}

void TaskProcess::getTask(Task task){
    queue.enqueue(task);
    if (!isProcessing) {
        emit processNext();
    }
}

void TaskProcess::startProcessing() {
    isProcessing = true;
    while (true) {
        Task task;
        {
            if (queue.isEmpty()) {
                isProcessing = false;
                qDebug() << "Запрос обработан";
                break;
            }
            task = queue.dequeue();
        }
        for (int i=0;i<task.files.size();i++) {
            fileProcess(task.sett, task.files[i]);
            bar1->setValue(i/task.files[i].size()*100.0);
        }
        bar1->setValue(100.0);
    }
    isProcessing = false;
}

void TaskProcess::fileProcess(Settings sett, QString fileName) {
    QFile file(sett.dir+'/'+fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << sett.dir+fileName;
        qDebug() << "Ошибка открытия файла:" << file.errorString();
        return;
    }

    QByteArray fileData = file.readAll();
    if (fileData.isEmpty()) {
        file.close();
        return;
    }

    for (int i = 0; i < fileData.size(); i++) {
        fileData[i] = fileData[i] ^ sett.key[i % sett.key.size()];
        bar2->setValue(i/fileData.size()*100.0);
    }
    QString filePath;
    if (sett.isOverwriting) {
        filePath = sett.dir2 +"/res_"+fileName;
    }
    else {
        filePath = uniqFileName(sett.dir2 +"/res_"+fileName);
    }

    QFile res(filePath);
    if (!res.open(QIODevice::WriteOnly)) {
        qDebug() << "Ошибка создания файла:" << file.errorString();
    }
    res.write(fileData);
    res.close();
    bar2->setValue(100.0);
    file.close();
    if (sett.needDelete){
        QFile::remove(sett.dir+fileName);
    }
}

QString TaskProcess::uniqFileName(QString originalPath) {
    QFileInfo fileInfo(originalPath);
    QString baseName = fileInfo.completeBaseName();
    QString suffix = fileInfo.suffix();
    QString path = fileInfo.path();

    if (!fileInfo.exists()) {
        return originalPath;
    }

    int counter = 1;
    QString newPath;

    do {
        newPath = QString("%1/%2_%3.%4")
        .arg(path)
            .arg(baseName)
            .arg(counter)
            .arg(suffix);
        counter++;
    } while (QFileInfo::exists(newPath));

    return newPath;
}
