#pragma once

#include <QWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QMap>
#include <QVector>
#include <QJsonDocument>
#include <QJsonArray>

#include "FileEntry.h"

class PageActions : public QWidget
{
    Q_OBJECT

public:
    explicit PageActions(QWidget *parent = nullptr);

    void loadGroups(const QMap<int, QVector<FileEntry>> &groups);

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

    static QString formatSize(qint64 bytes);
};
