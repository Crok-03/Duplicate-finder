#pragma once
#include <QObject>
#include <atomic>
#include <QStringList>

class ScanWorker : public QObject
{
    Q_OBJECT

public:
    explicit ScanWorker(QObject *parent = nullptr);

    void setFolders(const QStringList &folders);

signals:
    void progressChanged(int value);
    void currentFileChanged(const QString &file);
    void statsChanged(int scanned, int duplicates);
    void finished();
    void canceled();

public slots:
    void startScan();
    void requestCancel();

private:
    QStringList folders;

    std::atomic<bool> cancelFlag = false;
};
