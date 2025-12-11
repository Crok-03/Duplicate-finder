#pragma once
#include <QString>

class Logger
{
public:
    static void info(const QString& msg);
    static void error(const QString& msg);
};
