#pragma once
#include <QString>
#include <QByteArray>

class Hasher
{
public:
    static QByteArray computeFastHash(const QString &path);
    static QByteArray computeFullHash(const QString &path);
};
