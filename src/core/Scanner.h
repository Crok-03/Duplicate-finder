#pragma once
#include <QObject>
#include <QStringList>
#include <QVector>
#include "FileEntry.h"

class Scanner : public QObject
{
    Q_OBJECT

public:
    explicit Scanner(QObject* parent = nullptr);
    void setDirectories(const QStringList& dirs);
    
    QVector<FileEntry> scan();

signals:
    void progress(int percent, QString currentFile);

private:
    QStringList directories;
};
