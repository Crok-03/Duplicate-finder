#pragma once
#include <QObject>
#include <QVector>
#include <QMap>
#include "FileEntry.h"

using DuplicateGroup = QVector<FileEntry>;

class DuplicateFinder : public QObject
{
    Q_OBJECT

public:
    explicit DuplicateFinder(QObject* parent = nullptr);

    QVector<DuplicateGroup> findDuplicates(const QVector<FileEntry>& files);

signals:
    void progress(int percent);
};
