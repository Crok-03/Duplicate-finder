#pragma once
#include <QString>
#include <QFileInfo>

struct FileEntry
{
    QString path;
    qint64 size = 0;

    QByteArray fastHash;   // хеш первых 64 KB
    QByteArray fullHash;   // SHA-256 полного файла
};
