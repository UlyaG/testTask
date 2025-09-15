#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Settings.h"
#include "taskProcess.h"
#include <QThread>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_time_checkBox_clicked(bool checked);

    void on_mask_line_editingFinished();

    void on_overwrite_button_clicked();

    void on_new_button_clicked();

    void on_delete_checkBox_clicked(bool checked);

    void on_spinBox_valueChanged(int arg1);

    void on_key_line_editingFinished();

    void on_start_button_clicked();

    void on_dir_button_clicked();

    void on_dir_button_2_clicked();
signals:
    void sendTask(Task);
private:
    Ui::MainWindow *ui;
    Settings sett;
    void fileSearch();
    QStringList files;
    QTimer *Timer;
    //void filesProcess();
    void fileProcess(QString);
    QString uniqFileName(QString);
    QThread* taskProcess;
    TaskProcess* worker;

};
#endif // MAINWINDOW_H
