#include "PageActions.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QJsonObject>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

// ------------------ ФОРМАТИРОВАНИЕ РАЗМЕРА ------------------
QString PageActions::formatSize(qint64 bytes)
{
    double size = bytes;
    QStringList units = {"B", "KB", "MB", "GB", "TB"};

    int i = 0;
    while (size >= 1024.0 && i < units.size() - 1) {
        size /= 1024.0;
        i++;
    }

    return QString("%1 %2").arg(QString::number(size, 'f', 2)).arg(units[i]);
}

PageActions::PageActions(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *main = new QHBoxLayout(this);

    // ---------------- ЛЕВАЯ ПАНЕЛЬ ----------------
    groupList = new QListWidget(this);
    groupList->setFixedWidth(220);
    connect(groupList, &QListWidget::itemSelectionChanged,
            this, &PageActions::onGroupSelected);

    main->addWidget(groupList);

    // ---------------- ПРАВАЯ ПАНЕЛЬ ----------------
    QVBoxLayout *right = new QVBoxLayout();

    // Таблица
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
        QTableWidgetItem *chk = new QTableWidgetItem();
        chk->setCheckState(Qt::Checked);
        table->setItem(i, 0, chk);

        table->setItem(i, 1, new QTableWidgetItem(formatSize(files[i].size)));
        table->setItem(i, 2, new QTableWidgetItem(files[i].fullHash.toHex()));
        table->setItem(i, 3, new QTableWidgetItem(files[i].path));
    }
}

QVector<FileEntry> PageActions::getSelectedFiles() const
{
    QVector<FileEntry> selected;

    int groupRow = groupList->currentRow();
    if (groupRow < 0) return selected;

    int groupId = groupMap.keys()[groupRow];
    const auto &files = groupMap[groupId];

    for (int i = 0; i < table->rowCount(); i++)
    {
        if (table->item(i, 0)->checkState() == Qt::Checked)
            selected.append(files[i]);
    }

    return selected;
}

// ============================================================================
//                              ОПЕРАЦИИ
// ============================================================================

void PageActions::onDeleteTrash()
{
    QVector<FileEntry> files = getSelectedFiles();
    if (files.isEmpty()) return;

#ifdef _WIN32
    for (const FileEntry &f : files) {
        wchar_t wPath[MAX_PATH];
        QString q = f.path;
        q.replace("/", "\\");

        q.toWCharArray(wPath);
        wPath[q.length()] = 0;
        wPath[q.length() + 1] = 0; // двойной null

        SHFILEOPSTRUCTW op = {0};
        op.hwnd = nullptr;
        op.wFunc = FO_DELETE;
        op.pFrom = wPath;
        op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;

        SHFileOperationW(&op);
    }
#else
    QMessageBox::warning(this, "Недоступно",
                         "Удаление в корзину пока работает только в Windows.");
#endif

    QMessageBox::information(this, "Готово", "Файлы перемещены в корзину.");
}

void PageActions::onDeletePermanent()
{
    QVector<FileEntry> files = getSelectedFiles();
    if (files.isEmpty()) return;

    for (const FileEntry &f : files)
    {
        QFile::remove(f.path);
    }

    QMessageBox::information(this, "Готово", "Файлы удалены навсегда.");
}

void PageActions::onMoveFiles()
{
    QVector<FileEntry> files = getSelectedFiles();
    if (files.isEmpty()) return;

    QString dir = QFileDialog::getExistingDirectory(this, "Выберите папку");
    if (dir.isEmpty()) return;

    for (const FileEntry &f : files)
    {
        QString newPath = dir + "/" + QFileInfo(f.path).fileName();
        QFile::rename(f.path, newPath);
    }

    QMessageBox::information(this, "Готово", "Файлы перемещены.");
}

void PageActions::onExportJson()
{
    QVector<FileEntry> files = getSelectedFiles();
    if (files.isEmpty()) return;

    QJsonArray arr;

    for (const FileEntry &f : files)
    {
        QJsonObject obj;
        obj["path"] = f.path;
        obj["size"] = (double)f.size;
        obj["hash"] = QString(f.fullHash.toHex());

        arr.append(obj);
    }

    QJsonDocument doc(arr);

    QString file = QFileDialog::getSaveFileName(this, "Сохранить JSON", "", "*.json");
    if (file.isEmpty()) return;

    QFile out(file);
    out.open(QFile::WriteOnly);
    out.write(doc.toJson());
    out.close();

    QMessageBox::information(this, "Готово", "JSON экспортирован.");
}
