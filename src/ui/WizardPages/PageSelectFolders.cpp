#include "PageSelectFolders.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QLabel>

PageSelectFolders::PageSelectFolders(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Список директорий
    listFolders = new QListWidget(this);

    btnAdd = new QPushButton("Добавить папку");
    btnRemove = new QPushButton("Удалить");

    connect(btnAdd, &QPushButton::clicked, this, &PageSelectFolders::onAddFolder);
    connect(btnRemove, &QPushButton::clicked, this, &PageSelectFolders::onRemoveFolder);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(btnAdd);
    buttonsLayout->addWidget(btnRemove);

    // Параметры
    QLabel *lblMinSize = new QLabel("Минимальный размер файла (KB):");
    spinMinSize = new QSpinBox();
    spinMinSize->setMinimum(0);
    spinMinSize->setMaximum(1000000);
    spinMinSize->setValue(0);

    QLabel *lblExt = new QLabel("Расширения (например: *.png;*.jpg):");
    editExtensions = new QLineEdit("*.txt;*.jpg;*.png");

    layout->addWidget(new QLabel("Выбранные папки:"));
    layout->addWidget(listFolders);
    layout->addLayout(buttonsLayout);

    layout->addWidget(lblMinSize);
    layout->addWidget(spinMinSize);

    layout->addWidget(lblExt);
    layout->addWidget(editExtensions);

    setLayout(layout);
}

void PageSelectFolders::onAddFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Выбор папки");

    if (!dir.isEmpty())
        listFolders->addItem(dir);
}

void PageSelectFolders::onRemoveFolder()
{
    delete listFolders->takeItem(listFolders->currentRow());
}

QStringList PageSelectFolders::getSelectedFolders() const
{
    QStringList dirs;
    for (int i = 0; i < listFolders->count(); i++)
        dirs << listFolders->item(i)->text();
    return dirs;
}
