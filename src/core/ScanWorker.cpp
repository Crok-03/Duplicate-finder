#include "ScanWorker.h"

#include <QDirIterator>
#include <QThread>
#include "DuplicateFinder.h"   // реализация поиска дубликатов
#include "Hasher.h"            // если используешь (DuplicateFinder внутри может вызывать Hasher)

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
    cancelFlag.store(true);
}

void ScanWorker::startScan()
{
    QVector<FileEntry> files;
    int scanned = 0;

    // 1) Собираем список файлов
    for (const QString &folder : folders)
    {
        QDirIterator it(folder, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
        while (it.hasNext()) {
            if (cancelFlag.load()) {
                emit canceled();
                return;
            }

            QString path = it.next();
            QFileInfo fi(path);
            if (!fi.isFile()) continue;

            FileEntry e;
            e.path = path;
            e.size = fi.size();
            files.append(e);

            scanned++;
            // оповещаем каждые N файлов (или каждый файл)
            if (scanned % 10 == 0) {
                emit statsChanged(scanned, 0);
            }
        }
    }

    // Если нет файлов — сразу завершить
    if (files.isEmpty()) {
        emit finished();
        return;
    }

    // Простое прогресс-эмитирование при сборе: 20% от общего процесса
    emit progressChanged(10);

    // 2) Находим дубликаты (это может быть долго)
    auto groups = DuplicateFinder::findDuplicates(files);

    // 3) Отправляем результаты в UI
    emit resultsReady(groups);

    // 4) Финальный прогресс и завершение
    emit progressChanged(100);
    emit finished();
}
