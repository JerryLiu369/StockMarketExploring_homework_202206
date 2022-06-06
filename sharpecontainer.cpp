#include "sharpecontainer.h"

SharpeContainer::SharpeContainer(QString code,qreal sharpe):code(code),sharpe(sharpe)
{

}

QString SharpeContainer::getCode()
{
    return this->code;
}

qreal SharpeContainer::getSharpe()
{
    return this->sharpe;
}

bool operator<(const SharpeContainer& a, const SharpeContainer& b)
{
    return a.sharpe>b.sharpe;
}
