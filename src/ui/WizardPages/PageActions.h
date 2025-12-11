#pragma once

#include <QWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QMap>
#include <QVector>

#include "FileEntry.h"

class PageActions : public QWidget
{
    Q_OBJECT

public:
    explicit PageActions(QWidget *parent = nullptr);

    void loadGroups(const QMap<int, QVector<FileEntry>> &groups);

signals:
    void deleteToTrash(const QVector<FileEntry> &files);
    void deletePermanent(const QVector<FileEntry> &files);
    void moveFiles(const QVector<FileEntry> &files);
    void exportJson(const QVector<FileEntry> &files);

private slots:
    void onGroupSelected();
    void onDeleteTrash();
    void onDeletePermanent();
    void onMoveFiles();
    void onExportJson();

private:
    QListWidget *groupList;
    QTableWidget *table;
    QPushButton *btnTrash;
    QPushButton *btnDelete;
    QPushButton *btnMove;
    QPushButton *btnExport;

    QMap<int, QVector<FileEntry>> groupMap;

    QVector<FileEntry> getSelectedFiles() const;
};
