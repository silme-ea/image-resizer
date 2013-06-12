#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settingsdialog.h"

#include <QTextCodec>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QProgressDialog>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentMap>

#include <QDebug>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setSettings(Settings _s);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

    //settings
    static Settings s;

    static QString output_path;

    void addDirectory(QString directory);
    static void scaleImage(QString image_path);

private slots:
    void on_pushButton_Run_clicked();
    void on_pushButton_Settings_clicked();
    void on_toolButton_OutputDir_clicked();
    void on_pushButton_DeleteAll_clicked();
    void on_pushButton_Delete_clicked();
    void on_pushButton_AddDir_clicked();
    void on_pushButton_AddFiles_clicked();
};

#endif // MAINWINDOW_H
