#include "FileOperator.h"

FileOperator::FileOperator(QObject* parent)
    : QObject(parent)
{
}

bool FileOperator::moveToTrash(const QString& path)
{
    return false; // Реализация позже
}

bool FileOperator::deletePermanent(const QString& path)
{
    return false; // Реализация позже
}

bool FileOperator::moveTo(const QString& path, const QString& destDir)
{
    return false; // Реализация позже
}
