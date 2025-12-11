#pragma once
#include <QObject>
#include <QString>

class FileOperator : public QObject
{
    Q_OBJECT

public:
    explicit FileOperator(QObject* parent = nullptr);

    bool moveToTrash(const QString& path);
    bool deletePermanent(const QString& path);
    bool moveTo(const QString& path, const QString& destDir);
};
