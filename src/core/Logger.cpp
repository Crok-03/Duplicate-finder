#include "Logger.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>

static const QString LOG_FILE = "logs/app.log";

void Logger::info(const QString& msg)
{
    QFile file(LOG_FILE);
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return; // или можно вызвать qWarning()

    QTextStream out(&file);
    out << "[" << QDateTime::currentDateTime().toString() << "] INFO: " << msg << "\n";
}

void Logger::error(const QString& msg)
{
    QFile file(LOG_FILE);
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "[" << QDateTime::currentDateTime().toString() << "] ERROR: " << msg << "\n";
}
