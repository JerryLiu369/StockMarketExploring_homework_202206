# Introduction
This is a homework for Program-Designing II of RUC, written with QT    

Keywords: sorting, data visualization, k-means clustering
  
A screenshot:
![program](https://user-images.githubusercontent.com/86883627/172663371-954e4092-1372-4f21-98b9-ba4c43ed82e7.png)  

Sample data:  

![image](https://user-images.githubusercontent.com/86883627/172778910-b779de4d-fc8d-4378-9078-f8075f4e057c.png)

读入数据为csv格式，需包含前六列且顺序相同

# 功能及实现概述  
1. 排序  
先分离数据：按行遍历数据，以股票代码为键存入`QHash<QString,QVector<QString>>`，每450000行将缓冲区写入000001_SZ.txt,000002_SZ.txt...等小文件（以防使用内存过大），并形成一个有序的`QStringList`存储股票代码  
再排序：依股票代码字典序打开每个小文件，对文件内容排序，再写入output.txt  
2. 加快访问与指标计算  
按股票代码打开文件，按行遍历，按月份分类，写入一个QHash，每个键建立一个小文件，并进行指标计算，写入sharpe.txt  
![1](https://user-images.githubusercontent.com/86883627/174461964-95970b68-a128-4bd4-92af-d041350aba77.png)  
3. 价格可视化  
使用QTCharts完成  
![2](https://user-images.githubusercontent.com/86883627/174461972-0509293a-256f-475c-8cfd-7a762b338922.png)
4. 指标计算  
2中已完成  
5. 查询夏普指数  
使用QTCharts完成  
6. 预测  
预测采用arma模型，绘图使用QTCharts完成  
7. 夏普指数滚动  
制作计数器，调用5中的绘图  
8. kmeans聚类  
聚类使用开源算法dkm，使用QTCharts绘制  
![3](https://user-images.githubusercontent.com/86883627/174461976-afcc1216-15ac-4d3d-a013-5f545e1f0412.png)
