#include "tools.h"
#include<QMessageBox>
#include<QHash>
#include<windows.h>

QStringList datasplit(FileReader* parent,const QDir& split,const QString& stockdatafilename){
    QFile stockdata(stockdatafilename);
    if (!stockdata.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(parent,"警告","文件读取失败或未读取文件！");
        return QStringList();
    }
    //日期	股票代码	开盘价	最高价
    //最低价	收盘价	涨跌额	涨跌幅(%)	成交量(手)
    //成交金额(万元)	振幅(%)	换手率(%)
    stockdata.readLine();
    QStringList stocknames;
    int counter=0,done=0;
    while (!stockdata.atEnd()) {
        static QHash<QString,QStringList> buffer;
        QString line=stockdata.readLine();
        QStringList linels=line.split(',');
        QString stockname=linels[1];
        if(!stocknames.count(stockname))stocknames.append(stockname);
        buffer[stockname].append(line);
        counter++;
        if(counter>=10000){
            done++;
            for(auto &&stock:buffer.keys()){
                QFile stocknow(split.absoluteFilePath(stock+".txt"));
                if (!stocknow.open(QIODevice::Append | QIODevice::Text)){
                    QMessageBox::warning(parent,"警告","文件读取出现错误！");
                    return QStringList();
                }
                for(auto &&row:buffer[stock]){
                    stocknow.write(row.toLocal8Bit().data());
                }
            }
            counter=0;
            buffer.clear();
        }
    }
    return stocknames;
}
