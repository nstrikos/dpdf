#include "contentsDialog.h"
#include "ui_contentsDialog.h"

ContentsDialog::ContentsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContentsDialog)
{
    ui->setupUi(this);
}

ContentsDialog::~ContentsDialog()
{
    delete ui;
}

void ContentsDialog::setContents(QVector<QString> *titles)
{
    ui->listWidget->clear();

    for (int i = 0; i < titles->size(); i++) {
        QListWidgetItem *newItem = new QListWidgetItem(titles->at(i));
        ui->listWidget->addItem(newItem);
    }
}

void ContentsDialog::on_listWidget_itemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);

    emit contentSelected(ui->listWidget->currentRow());
}

void ContentsDialog::on_listWidget_itemPressed(QListWidgetItem *item)
{
    Q_UNUSED(item);

    emit contentSelected(ui->listWidget->currentRow());
}

void ContentsDialog::on_listWidget_itemActivated(QListWidgetItem *item)
{
    Q_UNUSED(item);

    emit contentSelected(ui->listWidget->currentRow());
}
