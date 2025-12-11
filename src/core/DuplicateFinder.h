#pragma once

#include "FileEntry.h"
#include <QVector>
#include <QMap>

class DuplicateFinder
{
public:
    // вход: сырые FileEntry без хешей
    // выход: группы дубликатов
    static QMap<int, QVector<FileEntry>> findDuplicates(QVector<FileEntry> &files);
};
