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
    QList<QPair<QString,QList<qreal>>> origin;
    QList<QPair<QString,QList<qreal>>> predict;
    for(int i=0;i<all.length();i++)
    {
        QStringList temp=all[i].split(',');
        origin.append(QPair<QString,QList<qreal>>(temp[0],{temp[2].toDouble(),temp[3].toDouble(),temp[4].toDouble(),temp[5].toDouble()}));
    }
    for (int i=6;i<origin.length();i++) {
        QList<qreal> temp{0,0,0,0};
        QList<QList<qreal>> coef=gencoef();
        /*
         * L1open L2open L3open L4open L5open L6open L1open2 L2open2 L3open2 L4open2 L5open2 L6open2
         * L1high L2high L3high L4high L5high L6high L1high2 L2high2 L3high2 L4high2 L5high2 L6high2
         * L1low L2low L3low L4low L5low L6low L1low2 L2low2 L3low2 L4low2 L5low2 L6low2
         * L1close L2close L3close L4close L5close L6close L1close2 L2close2 L3close2 L4close2 L5close2 L6close2
         */
        QList<qreal> predata{
            origin[i-1].second[0],origin[i-2].second[0],origin[i-3].second[0],
                    origin[i-4].second[0],origin[i-5].second[0],origin[i-6].second[0],
                    pow(origin[i-1].second[0],2),pow(origin[i-2].second[0],2),pow(origin[i-3].second[0],2),
                    pow(origin[i-4].second[0],2),pow(origin[i-5].second[0],2),pow(origin[i-6].second[0],2),
                    origin[i-1].second[1],origin[i-2].second[1],origin[i-3].second[1],
                    origin[i-4].second[1],origin[i-5].second[1],origin[i-6].second[1],
                    pow(origin[i-1].second[1],2),pow(origin[i-2].second[1],2),pow(origin[i-3].second[1],2),
                    pow(origin[i-4].second[1],2),pow(origin[i-5].second[1],2),pow(origin[i-6].second[1],2),
                    origin[i-1].second[2],origin[i-2].second[2],origin[i-3].second[2],
                    origin[i-4].second[2],origin[i-5].second[2],origin[i-6].second[2],
                    pow(origin[i-1].second[2],2),pow(origin[i-2].second[2],2),pow(origin[i-3].second[2],2),
                    pow(origin[i-4].second[2],2),pow(origin[i-5].second[2],2),pow(origin[i-6].second[2],2),
                    origin[i-1].second[3],origin[i-2].second[3],origin[i-3].second[3],
                    origin[i-4].second[3],origin[i-5].second[3],origin[i-6].second[3],
                    pow(origin[i-1].second[3],2),pow(origin[i-2].second[3],2),pow(origin[i-3].second[3],2),
                    pow(origin[i-4].second[3],2),pow(origin[i-5].second[3],2),pow(origin[i-6].second[3],2),
                    1
        };
        for (int i=0;i<4;i++) {
            temp[i]=dotmut(predata,coef[i]);
        }
        predict.append(QPair<QString,QList<qreal>>(origin[i].first,temp));
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
    int type=ui->K_type_2->text().toInt();
    if(type>4||type<1)
    {
        QMessageBox::warning(this,"类型错误","预测类型不存在！");
        return;
    }
    for (int i=std::max(0,origin.length()-nowlen);i<origin.length();i++) {
        ind++;
        seriesorigin->append(ind,origin[i].second[type-1]);
        seriespredict->append(ind,predict[i].second[type-1]);
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
    ui->S_table->setColumnCount(1);
    ui->S_table->setRowCount(30);
    ui->S_table->horizontalHeader()->setVisible(false);
    ui->S_table->verticalHeader()->setVisible(false);
    for(int i=0;i<std::min(30,fetch.length());i++)
    {
        SharpeContainer stock=fetch[i];
        ui->S_table->setItem(0,i,new QTableWidgetItem(stock.getCode()));
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

    chart->legend()->setVisible(false);

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
    for (const QString &month : data.keys()) {
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
        ui->S_table_2->setColumnCount(1);
        ui->S_table_2->setRowCount(30);
        ui->S_table_2->horizontalHeader()->setVisible(false);
        ui->S_table_2->verticalHeader()->setVisible(false);
        for(int i=0;i<std::min(30,fetch.length());i++)
        {
            SharpeContainer stock=fetch[i];
            ui->S_table_2->setItem(0,i,new QTableWidgetItem(stock.getCode()));
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

        chart->legend()->setVisible(false);

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

