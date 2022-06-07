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
    end_flag=true;
    delete ui;
}
#include<QMessageBox>
void kmeans::on_pushButton_clicked()
{
    end_flag=false;
    //读取数据
    QFile sharpe("sharpe.txt");
    sharpe.open(QIODevice::ReadOnly|QIODevice::Text);
    std::vector<std::array<double,2>> data;
    while(!sharpe.atEnd())
    {
        QStringList linels=QString(sharpe.readLine()).split(',');
        data.push_back({linels[3].toDouble(),linels[4].toDouble()});
    }
    sharpe.close();
    //聚类
    ui->pushButton->setText("聚类中...");
    QApplication::processEvents();
    auto labels=std::get<1>(dkm::kmeans_lloyd(data,9));
    ui->pushButton->setText("开始");
    QApplication::processEvents();
    //作图
    QChart *chart = new QChart();
    chart->setAnimationOptions(QChart::SeriesAnimations);
    //chart->legend()->setVisible(false);

    QList<QScatterSeries*> serieses;
    QList<QColor> colors{
        QColor("#000000"),
                QColor("#3DB3B7"),
                QColor("#B33E3C"),
                QColor("#78B43C"),
                QColor("#6D3CB4"),
                QColor("#D6AE84"),
                QColor("#70256D"),
                QColor("#325395"),
                QColor("#998433")
    };
    for(int i=0;i<9;i++){
        QScatterSeries *temp = new QScatterSeries();
        temp->setColor(colors[i]);
        temp->setMarkerSize(10.0);
        serieses.append(temp);
        chart->addSeries(temp);
    }
    chart->createDefaultAxes();
    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).at(0));
    axisX->setRange(-0.05,0.05);
    axisX->setLabelFormat("%.2f");

    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
    axisY->setRange(0,0.1);
    axisY->setLabelFormat("%.2f");

    ui->widget->setRenderHint(QPainter::Antialiasing);
    ui->widget->setChart(chart);

    for(size_t i=0;i<data.size();i++){
        if(end_flag)return;
        auto point=data[i];
        serieses[labels[i]]->append(QPointF(point[0],point[1]));
        QApplication::processEvents();
    }
}

void kmeans::on_pushButton_2_clicked()
{
    end_flag=true;
    this->close();
}
