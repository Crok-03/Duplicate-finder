#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>

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
    stack->addWidget(pageScan);

    pageResults = new PageResults(this);
    stack->addWidget(pageResults);

    // Заглушки для следующих шагов
    stack->addWidget(new QLabel("Шаг 4: Действия"));

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
}

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

        // Создаем поток
        scanThread = new QThread(this);
        worker = new ScanWorker();
        worker->setFolders(folders);
        worker->moveToThread(scanThread);

        connect(scanThread, &QThread::started, worker, &ScanWorker::startScan);

        // Обновление UI
        connect(worker, &ScanWorker::progressChanged,
                pageScan, &PageScan::setProgress);
        connect(worker, &ScanWorker::currentFileChanged,
                pageScan, &PageScan::setCurrentFile);
        connect(worker, &ScanWorker::statsChanged,
                pageScan, &PageScan::setStats);

        // Завершение сканирования → Шаг 3
       connect(worker, &ScanWorker::finished, this, [=]() {
        scanThread->quit();
        scanThread->wait();
        worker->deleteLater();
        scanThread->deleteLater();

        currentPage = 2;
        stack->setCurrentIndex(currentPage);
        updateStepTitle();

        // -----------------------------
        // ТЕСТОВЫЕ ДАННЫЕ ДЛЯ РЕЗУЛЬТАТОВ
        // -----------------------------
        pageResults->clearResults();

        pageResults->addResult({1, "15 KB", "ABC123", "C:/test/1.txt"});
        pageResults->addResult({1, "15 KB", "ABC123", "C:/test/2.txt"});
        pageResults->addResult({2, "2 MB", "ZZZ555", "C:/Photos/img1.jpg"});
        pageResults->addResult({2, "2 MB", "ZZZ555", "C:/Photos/img2.jpg"});
    });

        // Отмена
        connect(worker, &ScanWorker::canceled, this, [=]() {
            scanThread->quit();
            scanThread->wait();
            worker->deleteLater();
            scanThread->deleteLater();
            QMessageBox::information(this, "Отменено",
                                     "Сканирование отменено пользователем.");
        });

        // Кнопка отмены на самой странице
        connect(pageScan, &PageScan::cancelRequested,
                worker, &ScanWorker::requestCancel);

        // Переходим на шаг 2
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



void MainWindow::onBack()
{
    if (currentPage == 0)
        return;

    // На шаге 2 кнопка назад должна работать — НО!
    // если сканирование идет — запрещаем возвращаться
    if (currentPage == 1 && scanThread && scanThread->isRunning()) {
        QMessageBox::warning(this, "Ошибка",
                             "Нельзя вернуться назад во время сканирования.");
        return;
    }

    currentPage--;
    stack->setCurrentIndex(currentPage);
    updateStepTitle();
}


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
        btnBack->setEnabled(false); // нельзя назад пока идет сканирование
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
