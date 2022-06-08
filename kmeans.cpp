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
        qreal mean=linels[3].toDouble();
        qreal sd=linels[4].toDouble();
        if(mean>-0.06&&mean<0.06&&sd<0.12)data.push_back({mean,sd});
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
                QColor("#A27E36")
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

    /*
                             v4
-------------------------------------------------------------
      Percentiles      Smallest
 1%     -.023384        -.35985
 5%    -.0115851       -.349373
10%    -.0078976       -.325249       Obs             613,849
25%    -.0037067       -.324942       Sum of Wgt.     613,849

50%     .0000567                      Mean           .0004866
                        Largest       Std. Dev.      .0130231
75%     .0041332        1.28376
90%     .0091571        1.52169       Variance       .0001696
95%     .0132541        2.73128       Skewness       95.21884
99%     .0273964        4.56203       Kurtosis       28540.15

                             v5
-------------------------------------------------------------
      Percentiles      Smallest
 1%     .0073016       4.68e-07
 5%     .0112397       7.22e-07
10%     .0135353       7.84e-07       Obs             613,849
25%     .0180452       8.21e-07       Sum of Wgt.     613,849

50%     .0248626                      Mean           .0282546
                        Largest       Std. Dev.      .0213631
75%     .0343356         3.2273
90%     .0458472        3.32199       Variance       .0004564
95%     .0549695        4.61189       Skewness       68.11651
99%     .0837288        4.75981       Kurtosis       11569.69

     */

    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).at(0));
    axisX->setRange(-0.06,0.06);
    axisX->setTitleText("平均值");
    axisX->setLabelFormat("%.2f");

    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
    axisY->setRange(0,0.12);
    axisY->setTitleText("标准差");
    axisY->setLabelFormat("%.2f");

    ui->widget->setRenderHint(QPainter::Antialiasing);
    ui->widget->setChart(chart);

    int i=0;
    auto labelsiter=labels.begin();
    for(auto &&point : data){
        if(end_flag)return;
        serieses[*labelsiter]->append(QPointF(point[0],point[1]));
        i++;
        labelsiter++;
        if(i%1000==0){
            QApplication::processEvents();
        }
    }
}

void kmeans::on_pushButton_2_clicked()
{
    end_flag=true;
}
