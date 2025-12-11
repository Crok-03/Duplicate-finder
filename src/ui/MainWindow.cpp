#include "MainWindow.h"

#include "WizardPages/PageSelectFolders.h"
#include "WizardPages/PageScan.h"
#include "WizardPages/PageResults.h"
#include "core/ScanWorker.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Duplicate Finder");
    resize(900, 600);

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // Заголовок шага
    lblTitle = new QLabel("Шаг 1: Выбор папок");
    lblTitle->setAlignment(Qt::AlignCenter);
    lblTitle->setStyleSheet("font-size: 20px; font-weight: bold; padding: 10px;");

    // Страницы мастера
    stack = new QStackedWidget(this);

    pageSelectFolders = new PageSelectFolders(this);
    stack->addWidget(pageSelectFolders);   // page 0

    pageScan = new PageScan(this);
    stack->addWidget(pageScan);            // page 1

    pageResults = new PageResults(this);
    stack->addWidget(pageResults);         // page 2

    pageActions = new PageActions(this);
    stack->addWidget(pageActions);    // page 3

    // Навигационная панель
    btnBack = new QPushButton("⬅ Назад");
    btnNext = new QPushButton("Сканировать ➡");   // вместо Далее

    btnBack->setEnabled(false);

    connect(btnNext, &QPushButton::clicked, this, &MainWindow::onNext);
    connect(btnBack, &QPushButton::clicked, this, &MainWindow::onBack);

    QHBoxLayout *navLayout = new QHBoxLayout();
    navLayout->addWidget(btnBack);
    navLayout->addWidget(lblTitle);
    navLayout->addWidget(btnNext);

    layout->addWidget(stack);
    layout->addLayout(navLayout);

    setCentralWidget(central);

    // Инициализация состояний
    currentPage = 0;
    updateStepTitle();
}

// ---------------------- onNext ----------------------
void MainWindow::onNext()
{
    // -----------------------------------------
    // ШАГ 1 → ШАГ 2 (запуск сканирования)
    // -----------------------------------------
    if (currentPage == 0)
    {
        QStringList folders = pageSelectFolders->getSelectedFolders();

        if (folders.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Выберите хотя бы одну папку!");
            return;
        }

        // Создаем поток и воркер
        scanThread = new QThread(this);
        worker = new ScanWorker();
        worker->setFolders(folders);
        worker->moveToThread(scanThread);

        // Запуск сканирования при старте потока
        connect(scanThread, &QThread::started, worker, &ScanWorker::startScan);

        // Обновление UI (PageScan)
        connect(worker, &ScanWorker::progressChanged,
                pageScan, &PageScan::setProgress);
        connect(worker, &ScanWorker::currentFileChanged,
                pageScan, &PageScan::setCurrentFile);
        connect(worker, &ScanWorker::statsChanged,
                pageScan, &PageScan::setStats);

        // Результаты: заполняем PageResults и PageActions
        connect(worker, &ScanWorker::resultsReady, this,
                [=](const QMap<int, QVector<FileEntry>>& groups)
        {
            // -----------------------------
            // ЗАПОЛНЯЕМ ТАБЛИЦУ РЕЗУЛЬТАТОВ (Шаг 3)
            // -----------------------------
            pageResults->clearResults();

            for (auto groupId : groups.keys())
            {
                for (const FileEntry &f : groups[groupId])
                {
                    pageResults->addResult({
                        groupId,
                        QString::number(f.size) + " B",
                        QString(f.fullHash.toHex()),
                        f.path
                    });
                }
            }

            // -----------------------------
            // ЗАПОЛНЯЕМ СТРАНИЦУ ДЕЙСТВИЙ (Шаг 4)
            // -----------------------------
            pageActions->loadGroups(groups);
        });


        // Завершение сканирования → Шаг 3
        connect(worker, &ScanWorker::finished, this, [=]() {
            // Останавливаем поток/убираем воркера
            if (scanThread->isRunning()) {
                scanThread->quit();
                scanThread->wait();
            }
            worker->deleteLater();
            scanThread->deleteLater();

            currentPage = 2; // PageResults
            stack->setCurrentIndex(currentPage);
            updateStepTitle();
        });

        // Отмена
        connect(worker, &ScanWorker::canceled, this, [=]() {
            if (scanThread->isRunning()) {
                scanThread->quit();
                scanThread->wait();
            }
            worker->deleteLater();
            scanThread->deleteLater();
            QMessageBox::information(this, "Отменено",
                                     "Сканирование отменено пользователем.");
            // Возвращаемся на страницу выбора папок (0) или оставляем на странице сканирования — по желанию
            currentPage = 0;
            stack->setCurrentIndex(currentPage);
            updateStepTitle();
        });

        // Кнопка отмены на самой странице PageScan вызывает requestCancel
        connect(pageScan, &PageScan::cancelRequested,
                worker, &ScanWorker::requestCancel);

        // Переходим на шаг 1 -> 2 (сканирование)
        currentPage = 1;
        stack->setCurrentIndex(currentPage);
        updateStepTitle();

        scanThread->start();
        return;
    }

    // -----------------------------------------
    // ШАГ 2 → (запрещено переходить вручную)
    // переход на шаг 3 выполняется только worker->finished
    // -----------------------------------------

    // -----------------------------------------
    // ШАГ 3 → ШАГ 4
    // -----------------------------------------
    if (currentPage == 2)
    {
        currentPage = 3;
        stack->setCurrentIndex(currentPage);
        updateStepTitle();
        return;
    }

    // -----------------------------------------
    // ШАГ 4 → (дальше нет)
    // -----------------------------------------
}

// ---------------------- onBack ----------------------
void MainWindow::onBack()
{
    if (currentPage == 0)
        return;

    // Если в процессе сканирования — запрещаем возврат
    if (currentPage == 1 && scanThread && scanThread->isRunning()) {
        QMessageBox::warning(this, "Ошибка",
                             "Нельзя вернуться назад во время сканирования.");
        return;
    }

    // Просто уходим на предыдущую страницу
    currentPage--;
    stack->setCurrentIndex(currentPage);
    updateStepTitle();
}

// ---------------------- updateStepTitle ----------------------
void MainWindow::updateStepTitle()
{
    switch (currentPage)
    {
        case 0:
            lblTitle->setText("Шаг 1: Выбор папок");
            btnNext->setText("Сканировать ➡");
            btnBack->setEnabled(false);
            break;

        case 1:
            lblTitle->setText("Шаг 2: Сканирование");
            btnNext->setText("Далее ➡");
            // во время сканирования по умолчанию отключаем назад
            btnBack->setEnabled(false);
            break;

        case 2:
            lblTitle->setText("Шаг 3: Результаты");
            btnNext->setText("Далее ➡");
            btnBack->setEnabled(true);
            break;

        case 3:
            lblTitle->setText("Шаг 4: Действия");
            btnNext->setText("Готово");
            btnBack->setEnabled(true);
            break;

        default:
            lblTitle->setText("");
            break;
    }
}
