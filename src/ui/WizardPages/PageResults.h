#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

struct ResultItem
{
    int groupId;
    QString size;
    QString hash;
    QString path;
};

class PageResults : public QWidget
{
    Q_OBJECT

public:
    explicit PageResults(QWidget *parent = nullptr);
    void clearResults();
    void addResult(const ResultItem &item);

private:
    QTableWidget *table;
};
