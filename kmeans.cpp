#include "kmeans.h"
#include "ui_kmeans.h"

kmeans::kmeans(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kmeans)
{
    ui->setupUi(this);
}

kmeans::~kmeans()
{
    delete ui;
}

void kmeans::on_pushButton_clicked()
{

}

void kmeans::on_pushButton_2_clicked()
{
    this->close();
}
