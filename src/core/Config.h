#pragma once
#include <QStringList>
#include <QString>

class Config
{
public:
    QStringList includeDirs;
    qint64 minSize = 1;
    QStringList extensions;
    bool dryRun = false;

    void load();
    void save();
};
