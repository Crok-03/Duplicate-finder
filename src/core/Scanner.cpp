#include "Scanner.h"

Scanner::Scanner(QObject* parent)
    : QObject(parent)
{
}

void Scanner::setDirectories(const QStringList& dirs)
{
    directories = dirs;
}

QVector<FileEntry> Scanner::scan()
{
    QVector<FileEntry> result;

    // Логика будет позже
    return result;
}
