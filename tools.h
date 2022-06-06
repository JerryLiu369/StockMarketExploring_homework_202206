#ifndef TOOLS_H
#define TOOLS_H

#include "filereader.h"
#include<QString>
#include<QDir>

QStringList datasplit(FileReader* ithis,const QDir& dir,const QString& filename);

#endif // TOOLS_H
