#pragma once
#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>

class PageScan : public QWidget
{
    Q_OBJECT

public:
    explicit PageScan(QWidget *parent = nullptr);

    // Методы для обновления состояния
    void setProgress(int value);
    void setCurrentFile(const QString &file);
    void setStats(int scanned, int totalFound);

signals:
    void cancelRequested();

private:
    QProgressBar *progress;
    QLabel *lblCurrentFile;
    QLabel *lblStats;
    QPushButton *btnCancel;
};
