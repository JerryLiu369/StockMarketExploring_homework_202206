#ifndef KMEANS_H
#define KMEANS_H

#include <QDialog>

namespace Ui {
class kmeans;
}

class kmeans : public QDialog
{
    Q_OBJECT

public:
    explicit kmeans(QWidget *parent = nullptr);
    ~kmeans();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::kmeans *ui;
};

#endif // KMEANS_H
