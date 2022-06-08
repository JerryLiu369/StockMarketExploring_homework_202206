#ifndef KMEANS_H
#define KMEANS_H

#include <QDialog>

#include<QFile>
#include <QtCharts>
QT_CHARTS_USE_NAMESPACE
#include "dkm/dkm.hpp"

namespace Ui {
class kmeans;
}

class kmeans : public QDialog
{
    Q_OBJECT

public:
    explicit kmeans(QWidget *parent = nullptr);
    ~kmeans();

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::kmeans *ui;
    bool end_flag=true;
};

#endif // KMEANS_H
