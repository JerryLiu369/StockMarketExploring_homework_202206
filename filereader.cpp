#include "filereader.h"
#include "ui_filereader.h"

FileReader::FileReader(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileReader)
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    ui->setupUi(this);
}

FileReader::~FileReader()
{
    delete ui;
}

void FileReader::on_pushButton_clicked()
{
    split();
    sort();
    index();

    QMessageBox::information(this,"初始化完成","数据读入与排序已完成！");
    this->close();
}

void FileReader::split()
{
    ui->progressBar->setValue(0);

    QString stockdatafilename=QFileDialog::getOpenFileName(this, "打开文件",".","*.csv");
    ui->label->setText("初始化开始，请耐心等待......");

    //清空文件夹
    QDir split("split");
    if(split.exists())split.removeRecursively();
    split.mkdir(".");

    //打开文件，读第一行
    //日期	股票代码	开盘价	最高价
    //最低价	收盘价	涨跌额	涨跌幅(%)	成交量(手)
    //成交金额(万元)	振幅(%)	换手率(%)
    QFile stockdata(stockdatafilename);
    if (!stockdata.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(this,"警告","文件读取失败或未读取文件！");
        return;
    }
    stockdata.readLine();

    //按股票代码对数据分类，缓冲
    QStringList stocknames;
    QHash<QString,QVector<QString>> buffer;
    int counter=0;
    float done=0;
    while (!stockdata.atEnd())
    {
        QString line=stockdata.readLine();
        QString stockname=line.split(',')[1];
        if(!stocknames.count(stockname))stocknames.append(stockname);
        buffer[stockname].append(line);
        counter++;
        if(counter>=450000)
        {
            for(const QString &stock:buffer.keys())
            {
                QFile stocknow("split/"+stock+".txt");
                stocknow.open(QIODevice::Append | QIODevice::Text);
                for(const QString &row:buffer[stock]){
                    stocknow.write(row.toUtf8().constData());
                }
                stocknow.close();
                QApplication::processEvents();
            }
            buffer.clear();
            counter=0;
            done+=45.0/1180;
            ui->progressBar->setValue(done*done*100);
        }
    }
    //清空缓冲区
    for(const QString &stock:buffer.keys())
    {
        QFile stocknow("split/"+stock+".txt");
        stocknow.open(QIODevice::Append | QIODevice::Text);
        for(const QString &row:buffer[stock]){
            stocknow.write(row.toUtf8().constData());
        }
        stocknow.close();
    }
    buffer.clear();
    ui->progressBar->setValue(100);
    QApplication::processEvents();
    stockdata.close();


    if(stocknames.isEmpty())
    {
        QMessageBox::information(this,"初始化失败","数据为空，初始化失败！");
    }

    //写入股票列表
    stocknames.sort();
    QFile stocks("stocks.txt");
    stocks.open(QIODevice::WriteOnly|QIODevice::Text);
    for(const QString &stock : stocknames) {
        stocks.write((stock+"\n").toUtf8().constData());
    }
    stocks.close();
}

void FileReader::sort()
{
    //读入股票列表
    QFile stock("stocks.txt");
    stock.open(QIODevice::ReadOnly|QIODevice::Text);
    QStringList stocknames=QString(stock.readAll()).split('\n',QString::SkipEmptyParts);
    stock.close();
    //逐个读已分割文件，排序，写入output.txt；计算夏普指数，写入sharpe.txt
    int countall=stocknames.length();
    int count=0;
    QFile sorted("output.txt");
    if(sorted.exists())sorted.remove();
    sorted.open(QIODevice::Append|QIODevice::Text);
    QFile sharpe("sharpe.txt");
    if(sharpe.exists())sharpe.remove();
    sharpe.open(QIODevice::Append|QIODevice::Text);
    for(const QString &stock:stocknames)
    {
        //排序并写入
        QFile now("split/"+stock+".txt");

        now.open(QIODevice::ReadOnly | QIODevice::Text);
        QString data=now.readAll();
        QStringList lines=data.split('\n',QString::SkipEmptyParts);
        lines.sort();
        now.close();

        qreal closebefore=0;
        QMap<QString,QVector<qreal>> buffer;
        now.open(QIODevice::WriteOnly|QIODevice::Text);
        for(const QString &line : lines)
        {
            auto data=(line+"\n").toUtf8().constData();
            now.write(data);
            sorted.write(data);

            //收益率计算
            QStringList linels=line.split(',');
            qreal closenow=linels[5].toDouble();
            if(closebefore!=0){
                buffer[linels[0].left(7)].append((closenow/closebefore)-1);
            }
            closebefore=closenow;
        }
        now.close();

        //夏普指数
        for(const QString &mon : buffer.keys()) {
            int n=buffer[mon].length();
            if(n==0||n==1)continue;
            qreal sum=0,ssum=0;
            for(const qreal &num : buffer[mon]) {
                sum+=num;
                ssum+=pow(num,2);
            }
            //写入：月份，代码，夏普指数，均值，标准差
            if(ssum/n-pow(sum/n,2)==0)continue;
            sharpe.write((mon+","+stock+","+QString::number(sum/sqrt(n*ssum-sum*sum))+","+QString::number(sum/n)+","+QString::number(std::sqrt(ssum/n-pow(sum/n,2)))+"\n").toUtf8().data());
        }

        //进度条
        count++;
        ui->progressBar_2->setValue(100.0*count/countall);
        QApplication::processEvents();
    }
    sorted.close();
    sharpe.close();
}

void FileReader::index()
{
    //读入股票列表
    QFile stock("stocks.txt");
    stock.open(QIODevice::ReadOnly|QIODevice::Text);
    QStringList stocknames=QString(stock.readAll()).split('\n',QString::SkipEmptyParts);
    stock.close();

    //检查文件夹
    QDir index("index");
    if(!index.exists())index.mkpath(".");

    int countall=stocknames.length();
    int count=0;
    for(const QString &stock : stocknames)
    {
        QFile stockdata("split/"+stock+".txt");
        stockdata.open(QIODevice::ReadOnly | QIODevice::Text);
        //读入缓冲区
        QHash<QString,QVector<QString>> buffer;
        while (!stockdata.atEnd())
        {
            QString line=stockdata.readLine();
            QString yrmo=line.left(7);
            buffer[stock+"-"+yrmo].append(line);
        }
        //缓冲区写入磁盘
        for(const QString &name : buffer.keys()) {
            QFile now("index/"+name+".txt");
            now.open(QIODevice::WriteOnly|QIODevice::Text);
            for(const QString &line : buffer[name]) {
                now.write(line.toUtf8().data());
            }
            now.close();
        }
        //进度条
        count++;
        ui->progressBar_3->setValue(100.0*count/countall);
        QApplication::processEvents();
        stockdata.close();
    }
}

