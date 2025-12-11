#pragma once
#include <QString>
#include <QDateTime>

struct FileEntry
{
    QString path;
    qint64 size = 0;
    QDateTime modified;
    
    QString quickHash; 
    QString fullHash;  
};
