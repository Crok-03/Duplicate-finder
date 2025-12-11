#include "ScanWorker.h"
#include <QDirIterator>
#include <QThread>

ScanWorker::ScanWorker(QObject *parent)
    : QObject(parent)
{
}

void ScanWorker::setFolders(const QStringList &folders)
{
    this->folders = folders;
}

void ScanWorker::requestCancel()
{
    cancelFlag = true;
}

void ScanWorker::startScan()
{
    int scanned = 0;
    int duplicates = 0; // пока фиктивно

    // считаем общее число файлов
    int totalFiles = 0;
    for (const QString &folder : folders)
    {
        QDirIterator it(folder, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            it.next();
            if (it.fileInfo().isFile())
                totalFiles++;
        }
    }

    if (totalFiles == 0) {
        emit finished();
        return;
    }

    // основное сканирование
    for (const QString &folder : folders)
    {
        QDirIterator it(folder, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            if (cancelFlag.load()) {
                emit canceled();
                return;
            }

            QString filePath = it.next();

            if (!it.fileInfo().isFile())
                continue;

            scanned++;

            // обновляем UI
            emit currentFileChanged(filePath);
            emit statsChanged(scanned, duplicates);

            int progress = (double(scanned) / totalFiles) * 100.0;
            emit progressChanged(progress);

            QThread::msleep(5); // чтобы видеть прогресс
        }
    }

    emit finished();
}
