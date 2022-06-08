#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    callreader();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::callreader()
{
    FileReader reader;
    reader.show();
    reader.exec();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    S_2_running=false;
}

void MainWindow::on_action_triggered()
{
    callreader();
    QApplication::processEvents();
}

void MainWindow::on_K_button_clicked()
{
    //读入数据
    QFile data("index/"+ui->K_code_num->text()+"_"+ui->K_code_char->text().toUpper()+"-"+ui->K_yr->text()+"-"+QString("%1").arg(ui->K_mon->text(),2,QChar('0'))+".txt");
    if(!data.exists())
    {
        QMessageBox::warning(this,"读取错误","数据不存在或输入错误!\n\n请重新初始化或确认输入！");
        return;
    }
    data.open(QIODevice::ReadOnly|QIODevice::Text);
    QStringList lines=QString(data.readAll()).split('\n',QString::SkipEmptyParts);
    data.close();

    //数据处理与生成
    QStringList categories;

    QCandlestickSeries *nowSeries = new QCandlestickSeries();
    nowSeries->setIncreasingColor(QColor(Qt::red));
    nowSeries->setDecreasingColor(QColor(Qt::green));

    for (const QString &line : lines) {
        QStringList linels=line.split(",");

        QString day=linels.at(0).split('-',QString::SkipEmptyParts)[2];
        categories.append(day);
        const qreal timestamp=day.toInt();
        const qreal open = linels.at(2).toDouble();
        const qreal high = linels.at(3).toDouble();
        const qreal low = linels.at(4).toDouble();
        const qreal close = linels.at(5).toDouble();

        QCandlestickSet *lineset=new QCandlestickSet(timestamp);
        lineset->setOpen(open);
        lineset->setHigh(high);
        lineset->setLow(low);
        lineset->setClose(close);

        nowSeries->append(lineset);
    }


    //绘图
    QChart *chart = new QChart();
    chart->addSeries(nowSeries);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->createDefaultAxes();

    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(chart->axes(Qt::Horizontal).at(0));
    axisX->setCategories(categories);

    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
    axisY->setMax(axisY->max() * 1.01);
    axisY->setMin(axisY->min() * 0.99);

    chart->legend()->setVisible(false);

    ui->K_widget->setRenderHint(QPainter::Antialiasing);
    ui->K_widget->setChart(chart);
}

void MainWindow::on_K_button_2_clicked()
{
    //读入数据
    QString prefix="index/"+ui->K_code_num_2->text()+"_"+ui->K_code_char_2->text().toUpper()+"-";
    QString yr=ui->K_yr_2->text();
    QString mon=ui->K_mon_2->text();
    QFile now(prefix+yr+"-"+QString("%1").arg(mon,2,QLatin1Char('0'))+".txt");
    if(!now.exists())
    {
        QMessageBox::warning(this,"读取错误","数据不存在或输入错误!\n\n请重新初始化或确认输入！");
        return;
    }
    QFile before;
    if(mon.toInt()==1)
    {
        before.setFileName(prefix+QString::number(yr.toInt()-1)+"-"+"12"+".txt");
    }
    else
    {
        before.setFileName(prefix+yr+"-"+QString("%1").arg(QString::number(mon.toInt()-1),2,QLatin1Char('0'))+".txt");
    }
    QStringList all;
    now.open(QIODevice::ReadOnly|QIODevice::Text);
    all+=QString(now.readAll()).split('\n',QString::SkipEmptyParts);
    int nowlen=all.length();
    if(all.length()<7)
    {
        QMessageBox::warning(this,"数据错误","数据过少，无法预测！");
        return;
    }
    now.close();
    if(before.exists()){
        before.open(QIODevice::ReadOnly|QIODevice::Text);
        all=QString(before.readAll()).split('\n',QString::SkipEmptyParts)+all;
        before.close();
    }
    //预测数据
    QList<QPair<QString,qreal>> origin;
    QList<QPair<QString,qreal>> predict;
    for(int i=0;i<all.length();i++)
    {
        QStringList temp=all[i].split(',');
        origin.append(QPair<QString,qreal>(temp[0],temp[5].toDouble()));
    }
    for (int i=6;i<origin.length();i++) {
        qreal temp=0;
        temp+=0.95091870*origin[i-1].second;
        temp+=0.02809760*origin[i-2].second;temp+=0.00002270*pow(origin[i-2].second,2);
        temp+=0.02697790*origin[i-3].second;temp+=-0.00001790*pow(origin[i-3].second,2);
        temp+=-0.02451320*origin[i-4].second;
        temp+=0.00155620*origin[i-5].second;
        temp+=0.01668100*origin[i-6].second;temp+=-0.00000661*pow(origin[i-6].second,2);
        temp+=0.00078870;
        predict.append(QPair<QString,qreal>(origin[i].first,temp));
    }
    for (int i=0;i<6;i++) {
        origin.removeFirst();
    }
    //作图
    QLineSeries *seriesorigin = new QLineSeries();
    seriesorigin->setName("真实价格");
    QLineSeries *seriespredict = new QLineSeries();
    seriespredict->setName("预测价格");
    int ind=0;
    for (int i=std::max(0,origin.length()-nowlen);i<origin.length();i++) {
        ind++;
        seriesorigin->append(ind,origin[i].second);
        seriespredict->append(ind,predict[i].second);
    }

    QChart *chart=new QChart();
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->addSeries(seriesorigin);
    chart->addSeries(seriespredict);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->createDefaultAxes();

    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).at(0));
    axisX->setRange(0,ind+1);
    axisX->setTickCount(ind+2);
    axisX->setLabelFormat("%d");

    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
    axisY->setMax(axisY->max() * 1.01);
    axisY->setMin(axisY->min() * 0.99);
    axisY->setLabelFormat("%.2f");

    ui->K_widget_2->setRenderHint(QPainter::Antialiasing);
    ui->K_widget_2->setChart(chart);
}

void MainWindow::on_S_button_clicked()
{
    //读入数据
    QFile sharpe("sharpe.txt");
    QHash<QString,QList<SharpeContainer>> data;
    sharpe.open(QIODevice::ReadOnly|QIODevice::Text);
    while(!sharpe.atEnd())
    {
        QStringList linels=QString(sharpe.readLine()).split(',');
        data[linels[0]].append(SharpeContainer(linels[1],linels[2].toDouble()));
    }
    sharpe.close();
    //准备数据
    QList<SharpeContainer> fetch=data[ui->S_yr->text()+"-"+QString("%1").arg(ui->S_mon->text(),2,QLatin1Char('0'))];
    if(fetch.isEmpty())
    {
        QMessageBox::warning(this,"读取错误","数据不存在或输入错误!\n\n请重新初始化或确认输入！");
        return;
    }
    std::sort(fetch.begin(),fetch.end());
    QBarSeries *series = new QBarSeries();
    for(int i=0;i<std::min(10,fetch.length());i++)
    {
        SharpeContainer stock=fetch[i];
        QBarSet *tempset=new QBarSet(stock.getCode());
        tempset->append(stock.getSharpe());
        series->append(tempset);
    }
    //作图
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->createDefaultAxes();

    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(chart->axes(Qt::Horizontal).at(0));
    axisX->setCategories(QStringList());

    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
    axisY->setLabelFormat("%.2f");

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    ui->S_widget->setRenderHint(QPainter::Antialiasing);
    ui->S_widget->setChart(chart);
}

void MainWindow::on_S_2_start_clicked()
{
    S_2_running=true;
    ui->S_2_status->setText("准备中...");
    ui->S_2_status->setStyleSheet(QString("background-color: rgb(85, 255, 0);"));

    //读入数据
    QFile sharpe("sharpe.txt");
    QHash<QString,QList<SharpeContainer>> data;
    sharpe.open(QIODevice::ReadOnly|QIODevice::Text);
    while(!sharpe.atEnd())
    {
        QStringList linels=QString(sharpe.readLine()).split(',');
        data[linels[0]].append(SharpeContainer(linels[1],linels[2].toDouble()));
    }
    sharpe.close();
    for (QString &month : data.keys()) {
        std::sort(data[month].begin(),data[month].end());
    }
    ui->S_2_status->setText("滚动中...");

    int yr=ui->S_yr_2->text().toInt();
    int mon=ui->S_mon_2->text().toInt();
    while (S_2_running) {
        //滚动计数
        mon+=1;
        if(mon==12){
            yr+=1;
            mon=1;
        }
        if(yr==2023)yr=1991;
        QString yrstr=QString::number(yr);
        QString monstr=QString("%1").arg(QString::number(mon),2,QLatin1Char('0'));
        //准备数据
        QList<SharpeContainer> fetch=data[yrstr+"-"+monstr];
        if(fetch.isEmpty())
        {
            continue;
        }
        ui->S_yr_2->setText(yrstr);
        ui->S_mon_2->setText(monstr);
        std::sort(fetch.begin(),fetch.end());
        QBarSeries *series = new QBarSeries();
        for(int i=0;i<std::min(10,fetch.length());i++)
        {
            SharpeContainer stock=fetch[i];
            QBarSet *tempset=new QBarSet(stock.getCode());
            tempset->append(stock.getSharpe());
            series->append(tempset);
        }
        //作图
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        chart->createDefaultAxes();

        QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(chart->axes(Qt::Horizontal).at(0));
        axisX->setCategories(QStringList());

        QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
        axisY->setLabelFormat("%.2f");

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignRight);

        ui->S_widget_2->setRenderHint(QPainter::Antialiasing);
        ui->S_widget_2->setChart(chart);

        //延时模块
        QElapsedTimer t;
        t.start();
        while(t.elapsed()<2000)QCoreApplication::processEvents();
    }
}

void MainWindow::on_S_2_end_clicked()
{
    S_2_running=false;
    ui->S_2_status->setText("未滚动...");
    ui->S_2_status->setStyleSheet(QString("background-color: rgb(85, 255, 255);"));
}

void MainWindow::on_actionK_meansjullei_triggered()
{
    callkmeans();
}

void MainWindow::callkmeans()
{
    kmeans means;
    means.show();
    means.exec();
}

