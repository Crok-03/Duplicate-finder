#pragma once

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QVector>
#include <atomic>

#include "FileEntry.h"            // <- обязательно
#include <QThread>                // для forward-declare/использования типов (не обязательно, но безопасно)

class ScanWorker : public QObject
{
    Q_OBJECT

public:
    explicit ScanWorker(QObject *parent = nullptr);

    void setFolders(const QStringList &folders);

signals:
    void progressChanged(int value);                 // 0..100
    void currentFileChanged(const QString &file);
    void statsChanged(int scanned, int duplicates);
    void finished();
    void canceled();
    void resultsReady(const QMap<int, QVector<FileEntry>> &groups);

public slots:
    void startScan();
    void requestCancel();

private:
    QStringList folders;
    std::atomic<bool> cancelFlag{false};
};
