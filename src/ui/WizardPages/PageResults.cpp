#include "PageResults.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QColor>

PageResults::PageResults(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    table = new QTableWidget(this);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"Группа", "Размер", "Хеш", "Путь"});

    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    layout->addWidget(table);
    setLayout(layout);
}

void PageResults::clearResults()
{
    table->setRowCount(0);
}

void PageResults::addResult(const ResultItem &item)
{
    int row = table->rowCount();
    table->insertRow(row);

    table->setItem(row, 0, new QTableWidgetItem(QString::number(item.groupId)));
    table->setItem(row, 1, new QTableWidgetItem(item.size));
    table->setItem(row, 2, new QTableWidgetItem(item.hash));
    table->setItem(row, 3, new QTableWidgetItem(item.path));

    // Цвет по номеру группы
    QColor groupColor = QColor::fromHsv((item.groupId * 35) % 360, 255, 230);

    for (int col = 0; col < 4; col++)
        table->item(row, col)->setBackground(groupColor);
}
