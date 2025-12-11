#include "Hasher.h"
#include <QCryptographicHash>
#include <QFile>

QByteArray Hasher::computeFastHash(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly))
        return {};

    QByteArray data = file.read(64 * 1024); // 64 KB
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256);
}

QByteArray Hasher::computeFullHash(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly))
        return {};

    QCryptographicHash hash(QCryptographicHash::Sha256);

    while (!file.atEnd())
        hash.addData(file.read(1024 * 256)); // читаем блоками по 256КБ

    return hash.result();
}
