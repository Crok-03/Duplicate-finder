#include "PageActions.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>

PageActions::PageActions(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *main = new QHBoxLayout(this);

    // ---------------- ЛЕВАЯ ПАНЕЛЬ ----------------
    groupList = new QListWidget(this);
    groupList->setFixedWidth(200);
    connect(groupList, &QListWidget::itemSelectionChanged,
            this, &PageActions::onGroupSelected);

    main->addWidget(groupList);

    // ---------------- ПРАВАЯ ПАНЕЛЬ ----------------
    QVBoxLayout *right = new QVBoxLayout();

    table = new QTableWidget(this);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"✔", "Размер", "Хеш", "Путь"});
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    table->setSelectionMode(QAbstractItemView::NoSelection);

    right->addWidget(table);

    // ---------------- КНОПКИ ----------------
    QHBoxLayout *btns = new QHBoxLayout();

    btnTrash = new QPushButton("Удалить в корзину");
    btnDelete = new QPushButton("Удалить навсегда");
    btnMove = new QPushButton("Переместить…");
    btnExport = new QPushButton("Экспорт JSON");

    connect(btnTrash, &QPushButton::clicked, this, &PageActions::onDeleteTrash);
    connect(btnDelete, &QPushButton::clicked, this, &PageActions::onDeletePermanent);
    connect(btnMove, &QPushButton::clicked, this, &PageActions::onMoveFiles);
    connect(btnExport, &QPushButton::clicked, this, &PageActions::onExportJson);

    btns->addWidget(btnTrash);
    btns->addWidget(btnDelete);
    btns->addWidget(btnMove);
    btns->addWidget(btnExport);

    right->addLayout(btns);
    main->addLayout(right);
}

void PageActions::loadGroups(const QMap<int, QVector<FileEntry>> &groups)
{
    groupMap = groups;
    groupList->clear();

    for (auto id : groups.keys())
    {
        groupList->addItem(QString("Группа %1 (%2 файлов)")
                           .arg(id)
                           .arg(groups[id].size()));
    }
}

void PageActions::onGroupSelected()
{
    table->clearContents();
    table->setRowCount(0);

    int row = groupList->currentRow();
    if (row < 0) return;

    int groupId = groupMap.keys()[row];
    const auto &files = groupMap[groupId];

    table->setRowCount(files.size());

    for (int i = 0; i < files.size(); i++)
    {
        // checkbox
        QTableWidgetItem *chk = new QTableWidgetItem();
        chk->setCheckState(Qt::Checked);
        table->setItem(i, 0, chk);

        table->setItem(i, 1, new QTableWidgetItem(QString::number(files[i].size)));
        table->setItem(i, 2, new QTableWidgetItem(files[i].fullHash.toHex()));
        table->setItem(i, 3, new QTableWidgetItem(files[i].path));
    }
}

QVector<FileEntry> PageActions::getSelectedFiles() const
{
    QVector<FileEntry> selected;

    int rowGroup = groupList->currentRow();
    if (rowGroup < 0) return selected;

    int groupId = groupMap.keys()[rowGroup];
    const auto &files = groupMap[groupId];

    for (int i = 0; i < table->rowCount(); i++)
    {
        if (table->item(i, 0)->checkState() == Qt::Checked)
            selected.append(files[i]);
    }
    return selected;
}

// ------------------------------ ACTION BUTTONS ------------------------------

void PageActions::onDeleteTrash()
{
    emit deleteToTrash(getSelectedFiles());
}

void PageActions::onDeletePermanent()
{
    emit deletePermanent(getSelectedFiles());
}

void PageActions::onMoveFiles()
{
    emit moveFiles(getSelectedFiles());
}

void PageActions::onExportJson()
{
    emit exportJson(getSelectedFiles());
}
