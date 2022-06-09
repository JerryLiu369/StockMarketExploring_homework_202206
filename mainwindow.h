#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include<QCandlestickSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts>
QT_CHARTS_USE_NAMESPACE
#include "filereader.h"
#include "sharpecontainer.h"
#include "kmeans.h"
#include "tools.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void on_action_triggered();

    void on_K_button_clicked();

    void on_K_button_2_clicked();

    void on_S_button_clicked();

    void on_S_2_start_clicked();

    void on_S_2_end_clicked();

    void on_actionK_meansjullei_triggered();

private:
    Ui::MainWindow *ui;
    void callreader();
    bool S_2_running=false;
    void callkmeans();
};
#endif // MAINWINDOW_H
