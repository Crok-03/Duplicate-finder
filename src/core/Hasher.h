#pragma once
#include <QObject>
#include <QString>

class Hasher : public QObject
{
    Q_OBJECT

public:
    explicit Hasher(QObject* parent = nullptr);
    QString computeSHA256(const QString& path);

signals:
    void progress(int percent);
};
