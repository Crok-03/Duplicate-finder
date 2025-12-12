#include "MainWindow.h"

#include "WizardPages/PageSelectFolders.h"
#include "WizardPages/PageScan.h"
#include "WizardPages/PageResults.h"
#include "WizardPages/PageActions.h"
#include "core/ScanWorker.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QThread>

#include <QDebug>
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

    // Страницы
    stack = new QStackedWidget(this);

    pageSelectFolders = new PageSelectFolders(this);
    stack->addWidget(pageSelectFolders);   // page 0

    pageScan = new PageScan(this);
    stack->addWidget(pageScan);            // page 1

    pageResults = new PageResults(this);
    stack->addWidget(pageResults);         // page 2

    pageActions = new PageActions(this);
    stack->addWidget(pageActions);         // page 3

    // Навигация
    btnBack = new QPushButton("⬅ Назад");
    btnNext = new QPushButton("Сканировать ➡");

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

    currentPage = 0;
    updateStepTitle();
}

// ---------------------- onNext ----------------------
void MainWindow::onNext()
{
    //
    // ШАГ 1 → ШАГ 2 (запуск сканирования)
    //
    if (currentPage == 0)
    {
        QStringList folders = pageSelectFolders->getSelectedFolders();

        if (folders.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Выберите хотя бы одну папку!");
            return;
        }

        // Создаём поток и воркер
        scanThread = new QThread(this);
        worker = new ScanWorker();
        worker->setFolders(folders);
        worker->moveToThread(scanThread);

        // Сигналы сканирования
        connect(scanThread, &QThread::started, worker, &ScanWorker::startScan);

        connect(worker, &ScanWorker::progressChanged,
                pageScan, &PageScan::setProgress);

        connect(worker, &ScanWorker::currentFileChanged,
                pageScan, &PageScan::setCurrentFile);

        connect(worker, &ScanWorker::statsChanged,
                pageScan, &PageScan::setStats);

        //
        // После готовности результатов
        //
        connect(worker, &ScanWorker::resultsReady,
                this, [=](const QMap<int, QVector<FileEntry>> &groups)
        {
            // Страница результатов
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

            // Страница действий
            pageActions->loadGroups(groups);
        });

        //
        // Завершение сканирования → переход на страницу 3
        //
        connect(worker, &ScanWorker::finished, this, [=]()
        {
            if (scanThread->isRunning()) {
                scanThread->quit();
                scanThread->wait();
            }

            worker->deleteLater();
            scanThread->deleteLater();
            worker = nullptr;
            scanThread = nullptr;

            currentPage = 2;
            stack->setCurrentIndex(2);
            updateStepTitle();
        });

        //
        // Отмена сканирования
        //
        connect(worker, &ScanWorker::canceled, this, [=]()
        {
            if (scanThread->isRunning()) {
                scanThread->quit();
                scanThread->wait();
            }

            worker->deleteLater();
            scanThread->deleteLater();
            worker = nullptr;
            scanThread = nullptr;

            QMessageBox::information(this, "Отменено",
                                     "Сканирование отменено пользователем.");

            currentPage = 0;
            stack->setCurrentIndex(0);
            updateStepTitle();
        });

        connect(pageScan, &PageScan::cancelRequested,
                worker, &ScanWorker::requestCancel);

        // Переключаемся на страницу сканирования
        currentPage = 1;
        stack->setCurrentIndex(1);
        updateStepTitle();

        scanThread->start();
        return;
    }

    //
    // ШАГ 2 — пропускается (управляется сканированием)
    //

    //
    // ШАГ 3 → ШАГ 4
    //
    if (currentPage == 2)
    {
        currentPage = 3;
        stack->setCurrentIndex(3);
        updateStepTitle();
        return;
    }

    //
    // ШАГ 4 → В НАЧАЛО
    //
    if (currentPage == 3)
    {
        currentPage = 0;
        stack->setCurrentIndex(0);
        updateStepTitle();
        return;
    }
}

// ---------------------- onBack ----------------------
void MainWindow::onBack()
{
    if (currentPage == 0)
        return;

    // Нельзя назад во время сканирования
    if (currentPage == 1 && scanThread && scanThread->isRunning()) {
        QMessageBox::warning(this, "Ошибка",
                             "Нельзя вернуться назад во время сканирования.");
        return;
    }

    // Шаг 3 → назад → сразу в начало
    if (currentPage == 2) {
        currentPage = 0;
        stack->setCurrentIndex(0);
        updateStepTitle();
        return;
    }

    // обычный переход
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
    }
}
