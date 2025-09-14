#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QTimer>
#include <QDir>
#include <QFileDialog>
#include <QFile>
#include <cstdint>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_time_checkBox_clicked(bool checked)
{
    if (checked) {
        ui->label_2->setEnabled(1);
        ui->label_3->setEnabled(1);
        ui->spinBox->setEnabled(1);
        sett.needTimer=1;
    }
    else {
        ui->label_2->setEnabled(0);
        ui->label_3->setEnabled(0);
        ui->spinBox->setEnabled(0);
        sett.needTimer=0;
    }
}

void MainWindow::on_mask_line_editingFinished()
{
    if (ui->mask_line->text()[0]=='.')
        sett.mask ='*'+ ui->mask_line->text();
    if (ui->mask_line->text()[ui->mask_line->text().length()-1]=='.')
        sett.mask = ui->mask_line->text() + '*';
    qDebug() << sett.mask;
}

void MainWindow::on_overwrite_button_clicked()
{
    sett.isOverwriting=1;
}

void MainWindow::on_new_button_clicked()
{
    sett.isOverwriting=0;
}

void MainWindow::on_delete_checkBox_clicked(bool checked)
{
    if (checked) {
        sett.needDelete=1;
    }
    else {
        sett.needDelete=0;
    }
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    sett.timerTime=arg1;
}

void MainWindow::on_key_line_editingFinished()
{
    sett.key = QByteArray::fromHex(ui->key_line->text().toLatin1());
}

void MainWindow::fileSearch(){

    QDir dir(sett.dir);
    files = dir.entryList(QStringList() << sett.mask, QDir::Files);
    if (files.isEmpty()) {
        ui->status_label->setText("Файл не найден");
    }
    else {
        if (sett.needTimer)
            Timer->stop();
        filesProcess();
    }
}

void MainWindow::fileProcess(QString fileName) {
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
        ui->file_progress->setValue(i/fileData.size()*100.0);
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
    ui->file_progress->setValue(100.0);
    file.close();
    if (sett.needDelete){
        QFile::remove(sett.dir+fileName);
    }
}

QString MainWindow::uniqFileName(QString originalPath) {
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

void MainWindow::filesProcess(){
    for (int i=0;i<files.size();i++) {
        fileProcess(files[i]);
        ui->files_progress->setValue(i/files.size()*100.0);
    }
    ui->files_progress->setValue(100);
}

void MainWindow::on_start_button_clicked()
{
    if (ui->key_line->text()=="" || sett.mask=="") {
        ui->status_label->setText("Не все поля заполнены");
    }
    else {
        ui->status_label->setText("");
        if (sett.needTimer) {
            Timer = new QTimer(this);
            connect(Timer, &QTimer::timeout, this, &MainWindow::fileSearch);
            Timer->start(60000*sett.timerTime);
            ui->status_label->setText("Ожидание работы таймера");
        }
        else {
            fileSearch();
        }

    }
}

void MainWindow::on_dir_button_clicked()
{
    sett.dir = QFileDialog::getExistingDirectory(this, "Выберите директорию", QDir::homePath(), QFileDialog::ShowDirsOnly);
}

void MainWindow::on_dir_button_2_clicked()
{
    sett.dir2 = QFileDialog::getExistingDirectory(this, "Выберите директорию", QDir::homePath(), QFileDialog::ShowDirsOnly);
}

