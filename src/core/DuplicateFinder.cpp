#include "DuplicateFinder.h"
#include "Hasher.h"

QMap<int, QVector<FileEntry>> DuplicateFinder::findDuplicates(QVector<FileEntry> &files)
{
    QMap<qint64, QVector<FileEntry>> bySize;

    // 1. группируем по размеру
    for (auto &f : files)
        bySize[f.size].append(f);

    QMap<int, QVector<FileEntry>> result;
    int groupId = 1;

    // 2. обрабатываем каждую группу
    for (auto group : bySize.values())
    {
        if (group.size() < 2)
            continue;

        // 3. быстрые хеши
        QMap<QByteArray, QVector<FileEntry>> byFastHash;

        for (auto &f : group)
        {
            f.fastHash = Hasher::computeFastHash(f.path);
            byFastHash[f.fastHash].append(f);
        }

        // 4. полные хеши
        for (auto fastGroup : byFastHash.values())
        {
            if (fastGroup.size() < 2)
                continue;

            QMap<QByteArray, QVector<FileEntry>> byFullHash;

            for (auto &f : fastGroup)
            {
                f.fullHash = Hasher::computeFullHash(f.path);
                byFullHash[f.fullHash].append(f);
            }

            // 5. отбираем реальные дубликаты
            for (auto fullGroup : byFullHash.values())
            {
                if (fullGroup.size() >= 2)
                {
                    result[groupId++] = fullGroup;
                }
            }
        }
    }

    return result;
}
