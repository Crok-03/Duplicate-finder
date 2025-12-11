#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>

#include "PageSelectFolders.h"
#include "WizardPages/PageScan.h"
#include "core/ScanWorker.h"
#include <QThread>
#include "WizardPages/PageResults.h"
#include "WizardPages/PageActions.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onNext();
    void onBack();

private:
    void updateStepTitle();

    QStackedWidget *stack;
    QPushButton *btnNext;
    QPushButton *btnBack;
    QLabel *lblTitle;
    PageScan *pageScan;
    PageResults *pageResults;
    PageActions *pageActions;

    // Страницы мастера
    PageSelectFolders *pageSelectFolders;

    int currentPage = 0;
    const int pageCount = 4;
private:
    QThread *scanThread = nullptr;
    ScanWorker *worker = nullptr;
};
