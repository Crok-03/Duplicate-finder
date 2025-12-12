#include "Logger.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>

static void ensureDir()
{
    QDir().mkpath("logs");
}

static const QString LOG_FILE = "logs/app.log";

void Logger::info(const QString &msg)
{
    ensureDir();

    QFile file(LOG_FILE);
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "[" << QDateTime::currentDateTime().toString(Qt::ISODate)
        << "] INFO: " << msg << "\n";
}

void Logger::error(const QString &msg)
{
    ensureDir();

    QFile file(LOG_FILE);
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "[" << QDateTime::currentDateTime().toString(Qt::ISODate)
        << "] ERROR: " << msg << "\n";
}
