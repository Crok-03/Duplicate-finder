#include "DuplicateFinder.h"

DuplicateFinder::DuplicateFinder(QObject* parent)
    : QObject(parent)
{
}

QVector<DuplicateGroup> DuplicateFinder::findDuplicates(const QVector<FileEntry>& files)
{
    QVector<DuplicateGroup> groups;

    // Реализация позже

    return groups;
}
