#include "PageScan.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

PageScan::PageScan(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    lblCurrentFile = new QLabel("Текущий файл: -");
    lblCurrentFile->setStyleSheet("font-size: 16px;");

    progress = new QProgressBar();
    progress->setRange(0, 100);
    progress->setValue(0);
    progress->setTextVisible(true);

    lblStats = new QLabel("Просканировано: 0 файлов\nНайдено дубликатов: 0");

    btnCancel = new QPushButton("Отменить");
    btnCancel->setStyleSheet("font-size: 14px;");

    connect(btnCancel, &QPushButton::clicked, this, &PageScan::cancelRequested);

    layout->addWidget(new QLabel("Сканирование файлов...", this));
    layout->addWidget(lblCurrentFile);
    layout->addWidget(progress);
    layout->addWidget(lblStats);
    layout->addWidget(btnCancel);

    layout->addStretch();
}

void PageScan::setProgress(int value)
{
    progress->setValue(value);
}

void PageScan::setCurrentFile(const QString &file)
{
    lblCurrentFile->setText("Текущий файл: " + file);
}

void PageScan::setStats(int scanned, int totalFound)
{
    lblStats->setText(
        QString("Просканировано: %1 файлов\nНайдено дубликатов: %2")
        .arg(scanned)
        .arg(totalFound)
    );
}
