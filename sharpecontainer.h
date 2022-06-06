#ifndef SHARPECONTAINER_H
#define SHARPECONTAINER_H

#include<QString>

class SharpeContainer
{
public:
    SharpeContainer(QString code,qreal sharpe);
    QString getCode();
    qreal getSharpe();
    friend bool operator<(const SharpeContainer& a, const SharpeContainer& b);
private:
    QString code;
    qreal sharpe;
};

bool operator<(const SharpeContainer& a, const SharpeContainer& b);

#endif // SHARPECONTAINER_H
