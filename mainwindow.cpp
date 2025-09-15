#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QTimer>
#include <QDir>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QThread* taskProcess = new QThread;
    worker = new TaskProcess(ui->files_progress, ui->file_progress);
    worker->moveToThread(taskProcess);

    connect(this, &MainWindow::sendTask, worker, &TaskProcess::getTask);
    connect(worker, &TaskProcess::processNext, worker, &TaskProcess::startProcessing);

    taskProcess->start();
}

MainWindow::~MainWindow()
{
    delete ui;
    taskProcess->quit();
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
        emit sendTask({sett, files});
        qDebug() << "Запрос отправлен";
        //filesProcess();
    }
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

