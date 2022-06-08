#include "kmeans.h"
#include "ui_kmeans.h"

kmeans::kmeans(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kmeans)
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    ui->setupUi(this);
}

kmeans::~kmeans()
{
    delete ui;
}

void kmeans::closeEvent(QCloseEvent *)
{
    end_flag=true;
}

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
    std::random_shuffle(data.begin(),data.end());
    sharpe.close();
    //聚类
    ui->pushButton->setText("聚类中...");
    QApplication::processEvents();
    auto labels=std::get<1>(dkm::kmeans_lloyd(data,9));
    ui->pushButton->setText("开始");
    QApplication::processEvents();
    //作图
    QChart *chart = new QChart();
    //chart->setAnimationOptions(QChart::SeriesAnimations);
    //chart->legend()->setVisible(false);

    QList<QScatterSeries*> serieses;
    QList<QColor> colors{
        QColor(Qt::black),
                QColor(Qt::cyan),
                QColor(Qt::red),
                QColor(Qt::green),
                QColor(Qt::magenta),
                QColor(Qt::yellow),
                QColor(Qt::gray),
                QColor(Qt::blue),
                QColor(Qt::darkBlue)
    };
    for(int i=0;i<9;i++){
        QScatterSeries *temp = new QScatterSeries();
        temp->setName(QString::number(i));
        temp->setColor(colors[i]);
        temp->setMarkerSize(10.0);
        serieses.append(temp);
        chart->addSeries(temp);
    }
    chart->createDefaultAxes();
    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).at(0));
    axisX->setRange(-0.1,0.1);
    axisX->setTitleText("平均值");
    axisX->setLabelFormat("%.2f");

    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
    axisY->setRange(0,0.2);
    axisY->setTitleText("标准差");
    axisY->setLabelFormat("%.2f");

    ui->widget->setRenderHint(QPainter::Antialiasing);
    ui->widget->setChart(chart);

    int i=0;
    for(auto &&point : data){
        if(end_flag)return;
        serieses[labels[i]]->append(QPointF(point[0],point[1]));
        i++;
        if(i%100==0){
            QApplication::processEvents();
        }
    }
}

void kmeans::on_pushButton_2_clicked()
{
    end_flag=true;
    this->close();
}
