#ifndef FILEREADER_H
#define FILEREADER_H

#include <QDialog>

#include<QMessageBox>
#include<QString>
#include<QFileDialog>
#include<QDir>
#include<math.h>
#include<QGraphicsDropShadowEffect>

namespace Ui {
class FileReader;
}

class FileReader : public QDialog
{
    Q_OBJECT

public:
    explicit FileReader(QWidget *parent = nullptr);
    ~FileReader();

private slots:
    void on_pushButton_clicked();

private:
    Ui::FileReader *ui;
    void split();
    void sort();
    void index();
};

#endif // FILEREADER_H
