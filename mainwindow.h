#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_checkRequest_toggled(bool checked);

    void on_lineEdit_textChanged(const QString &arg1);

    void request();

    void readData();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
