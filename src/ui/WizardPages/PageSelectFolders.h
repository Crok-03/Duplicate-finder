#pragma once
#include <QWidget>
#include <QListWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>

class PageSelectFolders : public QWidget
{
    Q_OBJECT
public:
    explicit PageSelectFolders(QWidget *parent = nullptr);

    QStringList getSelectedFolders() const;

private slots:
    void onAddFolder();
    void onRemoveFolder();

private:
    QListWidget *listFolders;
    QPushButton *btnAdd;
    QPushButton *btnRemove;

    QSpinBox *spinMinSize;
    QLineEdit *editExtensions;
};
